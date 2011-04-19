
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

typedef std::vector<Coords> container_type;
typedef std::pair< container_type , container_type > state_type;
//typedef boost::numeric::odeint::hamiltonian_stepper_rk<container_type> stepper_type;

class DerivClass {
    private:
        boost::shared_ptr<Force> x_;
    public:
        DerivClass(boost::shared_ptr<Force> x):x_(x){};

        void operator()(const container_type &pq, container_type &dpqdt, const double t) const {
            for (std::size_t i=0;i<pq.size();++i){
                x_->operator()(pq[i],dpqdt[i],t);
            }
        }
};

/*Cpair OdeIntSympStrategy::operator()(Cpair &xin){



    std::size_t half = xin.first.size()/2;
    Coords x(half), p(half);
    for (std::size_t i=0;i<half;++i){
        x[i] = xin.first[i];
        p[i] = xin.first[i+half];
    }

    container_type xx(1,x), pp(1,p);
    state_type state = std::make_pair(xx, pp);
    stepper_type stepper;
    double t(tin_);
    std::pair<DerivClass,DerivClass> derivs = std::make_pair(DerivClass(kin_),DerivClass(pot_));

    do{
        t+=dt_;
        stepper.do_step(derivs, state, t, dt_);
    }while(t<=xin.second);
    tin_ = t;
    Coords xfin(xin.first.size());
    for (std::size_t i=0;i<half;++i){
        xfin[i] = x[i];
        xfin[i+half] = p[i];
    }

    return std::make_pair(xfin,t);
}*/

Cpair SymplecticIntStrategy::operator()(Cpair &xin){
    double t(tin_);
    std::size_t half = xin.first.size()/2;

    Coords x(half), p(half), tempp(half,0), tempx(half,0);
    for (std::size_t i=0;i<half;++i){
        x[i] = xin.first[i];
        p[i] = xin.first[i+half];
    }
    //std::copy(xin.first.begin(),xin.first.begin()+half,x.begin());
    //std::copy(xin.first.begin()+half,xin.first.end(),p.begin());
    //TODO: something is wrong with my Point class that the above copy doesnt work!

    do{
        t+=dt_;
        for (int i=0;i<k;++i){
            //TODO: Optimise dt_ * c[i] at some point. doesn't need to be calculated at every loop
            kin_->operator()(p, tempp, t);
            x += dt_ * c[i] * tempp;
            pot_->operator()(x, tempx, t);
            p += dt_ * d[i] * tempx;
        }
        //DLOG(INFO) << t << " " << x << " " << p << std::endl;
    }while(t<=xin.second);
    tin_ = t;

    Coords xfin(xin.first.size());
    for (std::size_t i=0;i<half;++i){
        xfin[i] = x[i];
        xfin[i+half] = p[i];
    }
    //std::copy(x.begin(),x.end(),xfin.begin());
    //std::copy(p.begin(),p.end(),xfin.begin()+half);
    return std::make_pair(xfin,t);
}

Integrator::Integrator(anyMap &params){

    using boost::any_cast;
    double tin, dt;
    boost::shared_ptr<Force> pot,kin;
    int strat = any_cast<int>(params["int-type"]);
    //int n = any_cast<int>(params["ndim"]);
    //double abs_error = any_cast<double>(params["relerror"]);
    //double rel_error = any_cast<double>(params["abserror"]);
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
            intStrat_ = boost::shared_ptr<SymplecticIntStrategy>(new SymplecticIntStrategy(tin, dt, kin, pot));
            LOG(INFO) << "using custom simplectic integrator";
            break;
        case 3:
            tin = any_cast<double>(params["tinitial"]);
            dt = any_cast<double>(params["dt"]);
            pot = any_cast<boost::shared_ptr<Force> >(params["potPtr"]);
            kin = any_cast<boost::shared_ptr<Force> >(params["kinPtr"]);
            intStrat_ = boost::shared_ptr<OdeIntSympStrategy>(new OdeIntSympStrategy(tin, dt, kin, pot));
            LOG(INFO) << "using custom simplectic integrator";
            break;
        case 4:
            tin = any_cast<double>(params["tinitial"]);
            dt = any_cast<double>(params["dt"]);
            pot = any_cast<boost::shared_ptr<Force> >(params["potPtr"]);
            kin = any_cast<boost::shared_ptr<Force> >(params["kinPtr"]);
            intStrat_ = boost::shared_ptr<OdeIntRKStrategy>(new OdeIntRKStrategy(tin, dt, kin, pot));
            LOG(INFO) << "using custom simplectic integrator";
            break;
        default:
            LOG(FATAL) << "bad choice for integrator type, exiting...";
    }
}

}
