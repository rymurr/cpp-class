#include "pywrapper.hpp"

namespace classical{

void simulation::initTrajsBins(){
    LOG(INFO) << "building binner and integrator";
    using boost::any_cast;
    int pDim = any_cast<int>(map_->operator[]("ndim"))/2;
    int pNum = any_cast<int>(map_->operator[]("plot-nums"));
    double pRange = any_cast<double>(map_->operator[]("plot-ranges"));
    static double escapeRange = any_cast<double>(map_->operator[]("escape-range"));
    int simType = any_cast<int>(map_->operator[]("sim-type"));
    int num_fields = any_cast<int>((*map_)["nfield"]);
    forces_ = boost::shared_ptr<std::vector<boost::shared_ptr<Force> > >(new std::vector<boost::shared_ptr<Force> >);
    forces_->push_back(forceFactory(*map_));
    for (int i = 0;i < num_fields ; ++i){
        forces_->push_back(forceFactory(fieldVec_[i]));
    }
    totForce_ = forceFactory(forces_,forces_->size());
    kinForce_ = forceFactory(6);
    map_->operator[]("potPtr") = totForce_;
    map_->operator[]("kinPtr") = kinForce_;
    //std::cout << simType << std::endl;
    std::vector<int> N(pDim, pNum);
    std::vector<double> V(pDim, pRange);
    bins_ = boost::shared_ptr<Binner>(new Binner(N,V));
    initBins_ = boost::shared_ptr<Binner>(new Binner(N,V));
    int_ = boost::shared_ptr<Integrator>(new Integrator(*map_));
    trajs_ = boost::shared_ptr<Trajs>(new Trajs(simType, *map_, icgen_, int_, bins_, initBins_));
    LOG(INFO) << "Binner and integrator built";
}

void simulation::run_trajs(){
    this->initTrajsBins();
    LOG(INFO) << "running trajectories";
    trajs_->runTraj();
    LOG(INFO) << "trajectories complete";
}

void simulation::run_bins(){
    using boost::any_cast;
    LOG(INFO) << "running binner";
    if (!any_cast<bool>(map_->operator[]("no-bin")))
        trajs_->runBins();
    LOG(INFO) << "binner complete";
}

void simulation::potsNFields(){
    LOG(INFO) << "building potentials and fields";
    int num_fields = any_cast<int>((*map_)["nfield"]);
    pots_ = boost::shared_ptr<std::vector<boost::shared_ptr<Potential> > >(new std::vector<boost::shared_ptr<Potential> >);
    pots_->push_back(potentialFactory(*map_));
    for (int i = 0;i < num_fields ; ++i){
        fieldVec_.push_back(fieldFactory(*map_));
        pots_->push_back(potentialFactory(fieldVec_[i]));
    }
    totPot_ = potentialFactory(pots_);
    LOG(INFO) << "finding tunnel exit";
    FindZeros zeros(1,boost::any_cast<double>((*map_)["ip"]),0.,totPot_);
    Coords fz = zeros.retZeros();
    LOG(INFO) << "Exit found at:" << fz << std::endl;
    std::vector<double> means(boost::any_cast<int>((*map_)["ndim"]));
    ///TODO: there is ambiguity here about how exactly things are set up wrt ionization direction etc.
    ///The pieces are there but a standard needs to be set up (ionization always along 2 for example)
    ///so as to make the choice of the means array unambiguous.
    ///The handling of this part of the code is also pretty embarassing, will need to make it more general
    means.front() = fz[0];
    (*map_)["means"] = means;
    (*map_)["variance"] = static_cast<double>(1/sqrt(std::abs(means.front()/sqrt(2.*boost::any_cast<double>((*map_)["ip"])))));
    LOG(INFO) << "Variance (tau) is: " << boost::any_cast<double>(map_->operator[]("variance")) << std::endl;
    boost::shared_ptr<std::vector<double> > sigmas = boost::shared_ptr<std::vector<double> >(new std::vector<double>(boost::any_cast<int>((*map_)["ndim"])));
    for (std::size_t i=0;i<sigmas->size()/2;++i){
        (*sigmas)[i] = 0.5*boost::any_cast<double>((*map_)["variance"])*boost::any_cast<double>((*map_)["variance"]);
    }
    for (std::size_t i=sigmas->size()/2;i<sigmas->size();++i){
        (*sigmas)[i] = 0.5/boost::any_cast<double>((*map_)["variance"])/boost::any_cast<double>((*map_)["variance"]);
    }
    (*map_)["sigmas"] = sigmas;
    LOG(INFO) << "exit found and potentials built";
}

void simulation::ic_gen(){

    this->potsNFields();
    LOG(INFO) << "building initial conditions";
    icgen_ = boost::shared_ptr<icgenerator>(new icgenerator(*map_));
    LOG(INFO) << "initial conditions and potentials have been built";
}

}
