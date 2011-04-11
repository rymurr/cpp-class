
#include "integrator.hpp"

namespace classical{

static const double c[4] = {0.5/(2.-pow(2.,1./3.)),0.5*(1.-pow(2.,1./3.))/(2.-pow(2.,1./3.)),0.5*(1.-pow(2.,1./3.))/(2.-pow(2.,1./3.)),0.5/(2.-pow(2.,1./3.))};
static const double d[4] = {1./(2.-pow(2.,1./3.)),pow(2,1./3.)/(2.-pow(2.,1./3.)),1./(2.-pow(2.,1./3.)),0.};

Coords<double> SimplecticIntStrategy::operator()(Coords<double>& x, double tfin){
    double t(tin_);
    Coords<double> xfin(x);
    int half = x.size()/2+1;
    int whole = x.size();

    while(t!=tfin){
        t+=dt_;
        for (int i=1;i<=4;++i){
            for (int j=0;j<half;++j){
                xfin[j] = xfin[j] + t * c[i] * kin_->operator()(xfin, t);
            }
            //we may need to do these in parallel xfin is changing during the loop. may be wrong...
            for (int j=half;j<whole;++j){
                xfin[j] = xfin[j] - t * d[i] * pot_->operator()(xfin,t);
            }
        }
    }
    return xfin;
}

Integrator::Integrator(anyMap &params){

    using boost::any_cast;

    double tin = any_cast<double>(params["tinitial"]);
    double dt = any_cast<double>(params["dt"]);
    boost::shared_ptr<Potential> pot = any_cast<boost::shared_ptr<Potential> >(params["potPtr"]);
    boost::shared_ptr<Potential> kin = any_cast<boost::shared_ptr<Potential> >(params["kinPtr"]);
    int strat = any_cast<int>(params["int-type"]);
    switch(strat){
        case 1:
            intStrat_ = boost::shared_ptr<VoidIntStrategy>(new VoidIntStrategy());
            LOG(INFO) << "using void integrator, integrator will return the coordinate set it is passed without integrating";
            break;
        case 2:
            intStrat_ = boost::shared_ptr<SimplecticIntStrategy>(new SimplecticIntStrategy(tin, dt, kin, pot));
            LOG(INFO) << "using custom simplectic integrator";
            break;
#ifdef GSL
        case 3:
            intStrat_ = boost::shared_ptr<GSLIntStrategy>(new GSLIntStrategy());
            LOG(INFO) << "using GSL supplied Runge-Kutta integrator";
            break;
#endif
        default:
            LOG(FATAL) << "bad choice for integrator type, exiting...";
    }
}

}
