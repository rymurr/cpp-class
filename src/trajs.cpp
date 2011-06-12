
#include "trajs.hpp"
namespace classical{

Trajs::Trajs(int id, anyMap& params, boost::shared_ptr<icgenerator> gen, boost::shared_ptr<Integrator> integrate, boost::shared_ptr<Binner> bin, boost::shared_ptr<Binner> initBin):bin_(bin), initBin_(initBin), int_(integrate), gen_(gen){
    using boost::any_cast;
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
#ifdef MPI_FOUND
        case 5:
            run_=&Trajs::multiMapReduce;
            LOG(INFO) << "running MPI parallel without storing and with binning";
            break;
#endif
        default:
            LOG(FATAL) << "bad sim-type, can't continue";
    }
            
}

void Trajs::singleStore(){
    LOG(INFO) << "starting trajectories";
    nums_.reserve(numTrajs_);
    initNums_.reserve(numTrajs_);
    int half = dims_/2;
    std::size_t i = 0;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    boost::progress_display show_progress(numTrajs_, std::clog);
    while(i<numTrajs_){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        xo = int_->operator()(x);
        if (fabs(xo.second - x.second) < 1E-5){
            initBin_->operator()(x.first,w,half);
            bin_->operator()(xo.first,w,half);
            nums_.push_back(xo.first);
            initNums_.push_back(x.first);
        }
        ++i;
        ++show_progress;
    }
}

void Trajs::singleNoBin(){
    LOG(INFO) << "starting trajectories";
    nums_.reserve(numTrajs_);
    initNums_.reserve(numTrajs_);
    std::size_t i = 0;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    boost::progress_display show_progress(numTrajs_, std::clog);
    while(i<numTrajs_){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        xo = int_->operator()(x);
        std::cout << " from trajs singleNoBin: hello!" << std::endl;
        if (fabs(xo.second - x.second) < 1E-5){
            nums_.push_back(xo.first);
            initNums_.push_back(x.first);
            ws_.push_back(w);
        }
        ++i;
        ++show_progress;
    }
}

void Trajs::singleNoStore(){
    LOG(INFO) << "starting trajectories";
    std::size_t i = 0;
    int half = dims_/2;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
    double w =0;
    boost::progress_display show_progress(numTrajs_, std::clog);
    while(i<numTrajs_){
        x.second = t_;
        gen_->retIC(x.first);
        w = gen_->retWeight(x.first);
        //std::cout << "hello!" << w << std::endl;
        //std::cout << x.first << std::endl;
        xo = int_->operator()(x);
        //std::cout << xo.second << " " << x.second << std::endl;
        if (fabs(xo.second - x.second) < 1E-5){
            //std::cout << "CUNT!" << std::endl;
            initBin_->operator()(x.first,w,half);
            bin_->operator()(xo.first,w,half);
        }
        ++i;
        ++show_progress;
    }
}

#ifdef MPI_FOUND
void Trajs::multiMapReduce(){

    LOG(INFO) << "starting trajectories";
    int half = dims_/2;


    namespace mpi = boost::mpi;
    mpi::communicator world;
    std::size_t numEach = numTrajs_/world.size();
    boost::progress_display show_progress(numEach, std::clog);
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
        if (fabs(xo.second - x.second) < 1E-5){
            initBin_->operator()(x.first,w,half);
            bin_->operator()(xo.first,w,half);
        }
        ++i;
        if (world.rank() == 0){
            ++show_progress;
        }
    }
    if (world.rank() == 0){
        gen_->seek(numEach*world.size()-numEach);
        for(std::size_t i=numEach*world.size();i<numTrajs_;++i){
            x.second = t_;
            gen_->retIC(x.first);
            w = gen_->retWeight(x.first);
            xo = int_->operator()(x);
            if (fabs(xo.second - x.second) < 1E-5){
                initBin_->operator()(x.first,w,half);
                bin_->operator()(xo.first,w,half);
            }
        }
    }

    world.barrier();
    if (world.rank() == 0) {
        LOG(INFO) << "trajectories done, starting reduce";
        bPtr nB(bin_);

        mpi::all_reduce(world, bin_, nB, binAdd());
        bin_ = nB;
      } else {
        mpi::all_reduce(world, bin_, binAdd());
    }
    world.barrier();
    if (world.rank() == 0) {
        bPtr nB(initBin_);
        //std::vector<int> nBs = nB.shape();
        //std::for_each(nBs.begin(),nBs.end(),std::cout<< " poo " << boost::lambda::_1); std::cout << std::endl;
        mpi::all_reduce(world, initBin_, nB, binAdd());
        initBin_ = nB;
      } else {
        mpi::all_reduce(world, initBin_, binAdd());
    }



}
#endif

bPtr binAdd::operator()(bPtr rhs, const bPtr lhs){
    rhs->operator+=(*lhs);
    return rhs;
}

void Trajs::multiOMPNoStore(){
    LOG(INFO) << "starting trajectories";
    int half = dims_/2;
    Coords xx(dims_,0.), xxo(dims_,0.);
    std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);

    boost::progress_display show_progress(numTrajs_, std::clog);
    #pragma omp parallel default(shared)
    {
        #ifdef OPENMP_FOUND
        //omp_lock_t genlock, binlock;
        //omp_init_lock(&genlock);
        //omp_init_lock(&binlock);
        #endif

        #pragma omp for schedule(dynamic,100)
        for (int i=0;i<static_cast<int>(numTrajs_); ++i){
            Integrator Int(*int_);
            std::pair<Coords,double> x = std::make_pair(xx,t_), xo = std::make_pair(xxo,t_);
#ifdef OPENMP_FOUND
            //omp_set_lock(&genlock);
#endif
            double w;
        #pragma omp critical
            gen_->retIC(x.first);
        #pragma omp critical
            w = gen_->retWeight(x.first);
#ifdef OPENMP_FOUND
            //omp_unset_lock(&genlock);
#endif
            xo = Int(x);
            if (fabs(xo.second - x.second) < 1E-5){
#ifdef OPENMP_FOUND
                //omp_set_lock(&binlock);
#endif
#pragma omp critical
                bin_->operator()(xo.first,w,half);
#pragma omp critical
                initBin_->operator()(x.first,w,half);
#ifdef OPENMP_FOUND
                //omp_unset_lock(&binlock);
#endif
                ++show_progress;
            }
            ///TODO:try and get rid of the &binlock omp_set_lock.

        }
    }
}

}
