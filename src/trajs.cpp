
#include "trajs.hpp"
namespace classical{

Trajs::Trajs(int id, anyMap& params, boost::shared_ptr<icgenerator> gen, boost::shared_ptr<Integrator> integrate, boost::shared_ptr<Binner> bin):bin_(bin), int_(integrate), gen_(gen){
    using boost::any_cast;
    ///TODO: check these names!
    std::vector<int> trajs = any_cast<std::vector<int> >(params["dims"]);
    numTrajs_ = std::accumulate(trajs.begin(),trajs.end(),1,std::multiplies<int>());
    dims_ = trajs.size();
    t_ = any_cast<double>(params["tfinal"]);
    switch(id){
        case 1:
            run_=&Trajs::singleStore;
            LOG(INFO) << "runing serial trajectories and storing results after binning";
            break;
        case 2:
            run_=&Trajs::singleNoStore;
            LOG(INFO) << "runing serial trajectories results are not stored after binning";
            break;
        case 3:
            run_=&Trajs::singleNoBin;
            LOG(INFO) << "runing serial trajectories and storing results without binning";
            break;
        case 4:
            run_=&Trajs::multiOMPNoStore;
            LOG(INFO) << "runing OpenMP parallel without storing and with binning";
            break;
        default:
            LOG(FATAL) << "wrong or currently unimplemented";
    }
            
}

void Trajs::singleStore(){
    ///TODO: set capacity of vector now so there are no re-allocations.
    std::size_t i = 0;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    while(i<numTrajs_){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        xo = int_->operator()(x);
        bin_->operator()(xo.first,w);
        nums_.push_back(xo.first);
        ++i;
    }
}

void Trajs::singleNoBin(){
    ///TODO: set capacity of vector now so there are no re-allocations.
    std::size_t i = 0;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    while(i<numTrajs_){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        xo = int_->operator()(x);
        nums_.push_back(xo.first);
        ++i;
    }
}

void Trajs::singleNoStore(){
    std::size_t i = 0;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    while(i<numTrajs_){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        xo = int_->operator()(x);
        bin_->operator()(xo.first,w);
        ++i;
    }
}

void Trajs::multiOMPNoStore(){
    
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);

    #pragma omp parallel default(shared)
    {
        #ifdef NDEBUG
        omp_lock_t genlock, binlock;
        omp_init_lock(&genlock);
        omp_init_lock(&binlock);
        #endif
        ///TODO: change schedule parameters
        #pragma omp for schedule(dynamic)
        for (std::size_t i=0;i<numTrajs_; ++i){
            Integrator Int(*int_);
            std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
#ifdef NDEBUG
            omp_set_lock(&genlock);
#endif
            gen_->retIC(x.first);
            double w = gen_->retWeight(x.first);
#ifdef NDEBUG
            omp_unset_lock(&genlock);
#endif
            xo = Int(x);
#ifdef NDEBUG
            omp_set_lock(&binlock);
#endif
            bin_->operator()(xo.first,w);
#ifdef NDEBUG
            omp_unset_lock(&binlock);
#endif
        }
    }
}

}
