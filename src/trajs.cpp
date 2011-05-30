
#include "trajs.hpp"
namespace classical{

Trajs::Trajs(int id, anyMap& params, boost::shared_ptr<icgenerator> gen, boost::shared_ptr<Integrator> integrate, boost::shared_ptr<Binner> bin, boost::shared_ptr<Binner> initBin):bin_(bin), initBin_(initBin), int_(integrate), gen_(gen){
    using boost::any_cast;
    ///TODO: check these names!
    std::vector<int> trajs = any_cast<std::vector<int> >(params["dims"]);
    numTrajs_ = std::accumulate(trajs.begin(),trajs.end(),1,std::multiplies<int>());
    dims_ = trajs.size();
    t_ = any_cast<double>(params["tfinal"]);
    switch(id){
        case 1:
            run_=&Trajs::singleStore;
            LOG(INFO) << "running serial trajectories and storing results after binning";
            break;
        case 2:
            run_=&Trajs::singleNoStore;
            LOG(INFO) << "running serial trajectories results are not stored after binning";
            break;
        case 3:
            run_=&Trajs::singleNoBin;
            LOG(INFO) << "running serial trajectories and storing results without binning";
            break;
        case 4:
            run_=&Trajs::multiOMPNoStore;
            LOG(INFO) << "running OpenMP parallel without storing and with binning";
            break;
        default:
            run_=&Trajs::multiMapReduce;
            LOG(INFO) << "running MPI parallel without storing and with binning";
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
        if (fabs(xo.second - x.second) > 1E-5){
            initBin_->operator()(x.first,w);
        }
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
        initNums_.push_back(x.first);
        //was going through this file to make sure initial bins get filled. Will then add methods to access them from simulation class
        //after that I have to add params and test C++ side, then serialize stuff, python side, testing and GUI!
        ws_.push_back(w);
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

void Trajs::multiMapReduce(){
    int argc; char** argv;
    namespace mpi = boost::mpi;
    mpi::environment env(argc, argv);
    mpi::communicator world;
    std::size_t numEach = numTrajs_/world.size();
    gen_->seek(numEach*world.rank());
    std::size_t i = 0;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    while(i<numEach){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        xo = int_->operator()(x);
        bin_->operator()(xo.first,w);
        ++i;
    }
    if (world.rank() == 0){
        gen_->seek(numEach*world.size()-numEach);
        for(std::size_t i=numEach*world.size();i<numTrajs_;++i){
            x.second = t_;
            gen_->retIC(x.first);
            w = gen_->retWeight(x.first);
            xo = int_->operator()(x);
            bin_->operator()(xo.first,w);
        }
    }
    world.barrier();
    if (world.rank() == 0) {
        bPtr nB(bin_);
        //std::vector<int> nBs = nB.shape();
        //std::for_each(nBs.begin(),nBs.end(),std::cout<< " poo " << boost::lambda::_1); std::cout << std::endl;
        mpi::all_reduce(world, bin_, nB, binAdd());
        bin_ = nB;
      } else {
        mpi::all_reduce(world, bin_, binAdd());
    }


}

bPtr binAdd::operator()(bPtr rhs, const bPtr lhs){
    rhs->operator+=(*lhs);
    return rhs;
}

void Trajs::multiOMPNoStore(){
    
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);

    #pragma omp parallel default(shared)
    {
        #ifdef OPENMP_FOUND
        omp_lock_t genlock, binlock;
        omp_init_lock(&genlock);
        omp_init_lock(&binlock);
        #endif
        ///TODO: change schedule parameters
        #pragma omp for schedule(dynamic)
        for (int i=0;i<static_cast<int>(numTrajs_); ++i){
            Integrator Int(*int_);
            std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
#ifdef OPENMP_FOUND
            omp_set_lock(&genlock);
#endif
            gen_->retIC(x.first);
            double w = gen_->retWeight(x.first);
#ifdef OPENMP_FOUND
            omp_unset_lock(&genlock);
#endif
            xo = Int(x);
#ifdef OPENMP_FOUND
            omp_set_lock(&binlock);
#endif
            bin_->operator()(xo.first,w);
#ifdef OPENMP_FOUND
            omp_unset_lock(&binlock);
#endif
        }
    }
}

}
