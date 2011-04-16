
#include "integrator.hpp"

namespace classical{

//4th order symplectic integrator

static const double k = 4;
static const double c[4] = {0.5/(2.-pow(2.,1./3.)),0.5*(1.-pow(2.,1./3.))/(2.-pow(2.,1./3.)),0.5*(1.-pow(2.,1./3.))/(2.-pow(2.,1./3.)),0.5/(2.-pow(2.,1./3.))};
static const double d[4] = {1./(2.-pow(2.,1./3.)),-pow(2,1./3.)/(2.-pow(2.,1./3.)),1./(2.-pow(2.,1./3.)),0.};


//1st order symplectic integrator
/*
static const double k = 1;
static const double c[1] = {1.};
static const double d[1] = {1.};
*/

Cpair SimplecticIntStrategy::operator()(Cpair &xin){
    double t(tin_);
    int half = xin.first.size()/2;

    Coords x(half), p(half);
    std::copy(xin.first.begin(),xin.first.begin()+half,x.begin());
    std::copy(xin.first.begin()+half,xin.first.end(),p.begin());

    do{
        t+=dt_;
        for (int i=0;i<k;++i){
            x += dt_ * c[i] * kin_->operator()(p, t);
            p += dt_ * d[i] * pot_->operator()(x, t);
        }
        //DLOG(INFO) << t << " " << x << " " << p << std::endl;
    }while(t<=xin.second);
    tin_ = t;

    Coords xfin(xin.first.size());
    std::copy(x.begin(),x.end(),xfin.begin());
    std::copy(p.begin(),p.end(),xfin.begin()+half);
    return std::make_pair(xfin,t);
}

Cpair GSLIntStrategy::operator()(Cpair &xin){

    double t = tin_;

    while (t < xin.second)
      {
        int status = gsl_odeiv_evolve_apply (e, c, s,
                                             &sys,
                                             &t, xin.second,
                                             dt_, xin.first);

        if (status != GSL_SUCCESS)
            throw integrator_error() << err_info("status of GSL integrator is error");

      }
    return xin;

}

Integrator::Integrator(anyMap &params){

    using boost::any_cast;
    double tin, dt;
    boost::shared_ptr<Force> pot,kin;
    int strat = any_cast<int>(params["int-type"]);
    switch(strat){
        case 1:
            intStrat_ = boost::shared_ptr<VoidIntStrategy>(new VoidIntStrategy());
            LOG(INFO) << "using void integrator, integrator will return the coordinate set it is passed without integrating";
            break;
        case 2:
            tin = any_cast<double>(params["tinitial"]);
            dt = any_cast<double>(params["dt"]);
            pot = any_cast<boost::shared_ptr<Force> >(params["potPtr"]);
            kin = any_cast<boost::shared_ptr<Force> >(params["kinPtr"]);
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
