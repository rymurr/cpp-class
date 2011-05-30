#include "pywrapper.hpp"

namespace classical{

void simulation::initTrajsBins(){
    using boost::any_cast;
    int pDim = any_cast<int>(map_->operator[]("plot_dims"));
    int pNum = any_cast<int>(map_->operator[]("plot_nums"));
    double pRange = any_cast<double>(map_->operator[]("plot_ranges"));
    std::vector<int> N(pDim, pNum);
    std::vector<double> V(pDim, pRange);
    bins_ = boost::shared_ptr<Binner>(new Binner(N,V));
    int_ = boost::shared_ptr<Integrator>(new Integrator(*map_));
    trajs_ = boost::shared_ptr<Trajs>(new Trajs(1, *map_, icgen_, int_, bins_));
}

void simulation::run_trajs(){
    this->initTrajsBins();
    trajs_->runTraj();
}

void simulation::run_bins(){
    using boost::any_cast;
    if (any_cast<bool>(map_->operator[]("separate_bins")))
        trajs_->runBins();
}

void simulation::ic_gen(){
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
    std::vector<double> means(boost::any_cast<int>((*map_)["ndim"]));
    ///TODO: there is ambiguity here about how exactly things are set up wrt ionization direction etc.
    //The pieces are there but a standard needs to be set up (ionization always along 0 for example)
    //so as to make the choice of the means array unambiguous.
    means[0] = fz[3];
    (*map_)["means"] = means;
    ///TODO: I think i set it up so variance was Tau(tunneling time) must double check!
    (*map_)["variance"] = static_cast<double>(std::abs(means[0]/sqrt(2.*boost::any_cast<double>((*map_)["ip"]))));
    boost::shared_ptr<std::vector<double> > sigmas = boost::shared_ptr<std::vector<double> >(new std::vector<double>(boost::any_cast<int>((*map_)["ndim"])));
    ///TODO: double check that x=1/2tau and p=tau/2
    for (std::size_t i=0;i<sigmas->size();i+=2){
        (*sigmas)[i] = 0.5/boost::any_cast<double>((*map_)["variance"]);
    }
    for (std::size_t i=1;i<sigmas->size();i+=2){
        (*sigmas)[i] = 0.5*boost::any_cast<double>((*map_)["variance"]);
    }
    (*map_)["sigmas"] = sigmas;
    ///TODO: add weight-func parameter to parameter set
    (*map_)["weight-func"] = 2;

    LOG(INFO) << "building initial conditions";
    icgen_ = boost::shared_ptr<icgenerator>(new icgenerator(*map_));
    LOG(INFO) << "initial conditions and potentials have been built";
}

}
