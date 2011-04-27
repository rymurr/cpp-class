
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
typedef boost::numeric::odeint::hamiltonian_stepper_rk<container_type> stepper_type_symp;
typedef boost::numeric::odeint::stepper_rk5_ck<container_type> stepper_type_rk;
typedef boost::numeric::odeint::controlled_stepper_standard< stepper_type_rk > controller;

class DerivClassP {
    private:
        boost::shared_ptr<Force> x_, dx_;
        bool time_;
    public:
        DerivClassP(){};

        DerivClassP(boost::shared_ptr<Force> x, bool time = false):x_(x), time_(time){};

        void operator()(const container_type &pq, container_type &dpqdt) const {

            //std::cout << pq.size() << dpqdt.size() << pq[1].size() << dpqdt[1].size() << std::endl;
            if (time_){
                for (std::size_t i=0;i<pq.size()-1;++i){
                    x_->operator()(pq[i],dpqdt[i]);
                }
                dpqdt.back() = Coords(1,1.);
            } else {
                for (std::size_t i=0;i<pq.size();++i){
                    x_->operator()(pq[i],dpqdt[i]);
                }
            }
            //std::cout << pq.size() << dpqdt.size() << pq[1].size() << dpqdt[1].size() << std::endl;
            //std::cout << dpqdt[1][0] << std::endl;
            //throw;
        }
};

class DerivClassRK {
    private:
        boost::shared_ptr<Force> kin_, pot_;

    public:
        DerivClassRK(){};

        DerivClassRK(boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot):kin_(kin), pot_(pot){};

        void operator()(const container_type &pq, container_type &dpqdt, double t) const {

            std::cout << pq.size() << " " << dpqdt.size() << " " << pq[0].size() << " " << pq[1].size() << " " << dpqdt[0].size() << " " << dpqdt[1].size() << std::endl;
            //kin_->operator()(pq[1], dpqdt[0], t);
            dpqdt[0] = pq[1];
            pot_->operator()(pq[0], dpqdt[1], t);
            std::cout << "yelp " << dpqdt[0][0] << " " << dpqdt[0][1] << " " << dpqdt[1][0] << " " << dpqdt[1][1] << std::endl;
            std::cout << pq[0][0] << " " << pq[0][1] << " " << pq[1][0] << " " << pq[1][1] << " " << t << std::endl;
        }
};


class DerivClassR {
    private:
        boost::shared_ptr<Force> x_, dx_;
        bool time_;
    public:
        DerivClassR(){};

        DerivClassR(boost::shared_ptr<Force> x, boost::shared_ptr<Force> dx = boost::shared_ptr<Force>()):x_(x), dx_(dx), time_(true){if (!dx_) time_ = false;};

        void operator()(const container_type &pq, container_type &dpqdt) const {

            if (time_){
                dpqdt.back() = Coords(1,0.); Coords temp(1,0.);
                for (std::size_t i=0;i<pq.size()-1;++i){
                    x_->operator()(pq[i], dpqdt[i], pq.back()[0]);
                    dx_->operator()(pq[i], temp, pq.back()[0]);
                    //std::cout << temp[0]/pq[0][0] << " " << pq.back()[0] << std::endl;
                    dpqdt.back() += temp;
                }
            } else {
                for (std::size_t i=0;i<pq.size();++i){
                    x_->operator()(pq[i],dpqdt[i]);
                }
            }
        }
};

class obs{
    public:
        void operator()(double t, container_type &x, DerivClassRK &px){
            std::cout << x[0][0] << " " << x[0][1] << " " << x[1][0] << " " << x[1][1] << " " << t << std::endl;
        }
};
Cpair OdeIntRKStrategy::operator()(Cpair &xin){

    std::size_t half = xin.first.size()/2;
    Coords x(half), p(half);
    for (std::size_t i=0;i<half;++i){
        x[i] = xin.first[i];
        p[i] = xin.first[i+half];
    }

    //double t(tin_);
    container_type xx(1,x);

    DerivClassRK derivs;
    derivs = DerivClassRK(kin_, pot_);

    xx.push_back(p);
    stepper_type_rk stepper;
    controller control(eps_abs_, eps_rel_, 1.0, 1.0);

//    do{
//        t+=dt_;
        std::cout << xx[0][0] << " " << xx[0][1] << " " << xx[1][0] << " " << xx[1][1] << " " << tin_ << " " << xin.second << std::endl;
        obs observ;
        std::size_t steps = boost::numeric::odeint::integrate_adaptive(control, derivs, xx, tin_, xin.second, dt_, observ);
        std::cout << xx[0][0] << " " << xx[0][1] << " " << xx[1][0] << " " << xx[1][1] << " " << steps << std::endl;

//    }while(t<=xin.second);
    //tin_ = t;

    Coords xfin(xin.first.size());
    for (std::size_t i=0;i<half;++i){
        xfin[i] = xx[0][i];
        xfin[i+half] = xx[1][i];
    }

    return std::make_pair(xfin,xin.second);
}

Cpair OdeIntSympStrategy::operator()(Cpair &xin){

    std::size_t half = xin.first.size()/2;
    Coords x(half), p(half);
    for (std::size_t i=0;i<half;++i){
        x[i] = xin.first[i];
        p[i] = xin.first[i+half];
    }

    double t(tin_);
    container_type xx(1,x), pp(1,p);

    boost::shared_ptr<Force> dpot;
    std::pair<DerivClassP,DerivClassR> derivs;
    if (dpot_){
        dpot = forceFactory(potentialFactory(dpot_,1));
        Coords temp(1);
        dpot->operator()(x,temp,t);
        xx.push_back(Coords(1,t));
        pp.push_back(temp);
        derivs = std::make_pair(DerivClassP(kin_, true),DerivClassR(pot_,dpot));
    } else {
        derivs = std::make_pair(DerivClassP(kin_),DerivClassR(pot_));
    }
    state_type state = std::make_pair(xx, pp);
    stepper_type_symp stepper;


    do{
        t+=dt_;
        stepper.do_step(derivs, state, t, dt_);
        std::cout << state.first[0][0] << " " << state.first[0][1] << " " << state.second[0][0] << " " << state.second[0][1] << " " << state.first[1][0] << std::endl;

    }while(t<=xin.second);
    tin_ = t;

    Coords xfin(xin.first.size());
    for (std::size_t i=0;i<half;++i){
        xfin[i] = state.first[0][i];
        xfin[i+half] = state.second[0][i];
    }

    return std::make_pair(xfin,t);
}

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
    double tin, dt, eps_abs, eps_rel;
    boost::shared_ptr<Force> pot,kin;
    boost::shared_ptr<Field> dpot;
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
            LOG(INFO) << "using custom symplectic integrator";
            break;
        case 3:
            tin = any_cast<double>(params["tinitial"]);
            dt = any_cast<double>(params["dt"]);
            pot = any_cast<boost::shared_ptr<Force> >(params["potPtr"]);
            kin = any_cast<boost::shared_ptr<Force> >(params["kinPtr"]);
            try{
                dpot = any_cast<boost::shared_ptr<Field> >(params["dpotPtr"]);
            } catch(boost::bad_any_cast &e) {
                dpot = boost::shared_ptr<Field>();
            }
            intStrat_ = boost::shared_ptr<OdeIntSympStrategy>(new OdeIntSympStrategy(tin, dt, kin, pot, dpot));
            LOG(INFO) << "using ODEINT simplectic integrator";
            break;
        case 4:
            tin = any_cast<double>(params["tinitial"]);
            dt = any_cast<double>(params["dt"]);
            pot = any_cast<boost::shared_ptr<Force> >(params["potPtr"]);
            kin = any_cast<boost::shared_ptr<Force> >(params["kinPtr"]);
            eps_rel = any_cast<double>(params["relerr"]);
            eps_abs = any_cast<double>(params["abserr"]);
            intStrat_ = boost::shared_ptr<OdeIntRKStrategy>(new OdeIntRKStrategy(tin, dt, kin, pot, eps_abs, eps_rel));
            LOG(INFO) << "using odeint RK integrator";
            break;
        default:
            LOG(FATAL) << "bad choice for integrator type, exiting...";
    }
}

}
