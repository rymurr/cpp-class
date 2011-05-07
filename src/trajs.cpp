
#include "trajs.hpp"
namespace classical{

Trajs::Trajs(int id, anyMap& params, boost::shared_ptr<icgenerator> gen, boost::shared_ptr<Integrator> integrate, boost::shared_ptr<Binner> bin):bin_(bin), int_(integrate), gen_(gen){
    using boost::any_cast;
    //TODO: check these names!
    numTrajs_ = any_cast<int>(params["trajs"]);
    dims_ = any_cast<int>(params["dims"]);
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
    //TODO: set capacity of vector now so there are no re-allocations.
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
    //TODO: set capacity of vector now so there are no re-allocations.
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
    #pragma omp parallel shared(gen_,bin_,dims_,t_) private(int_,x,xo)
    {
        #pragma omp for schedule(dynamic)
        {
            //TODO: change schedule parameters
            for (std::size_t i=0;i<numTrajs_; ++i){
                x.second = t_;
                gen_->retIC(x.first);
                double w = gen_->retWeight(x.first);
                xo = int_->operator()(x);
                bin_->operator()(xo.first,w);
            }
        }
    }
    //TODO: I am very concerned about thread safety...this may need to be extensively changed to make sure the threads don't fuck things up. Must test on the bigger machine.

}

}
