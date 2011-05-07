
#include "integrator.hpp"

namespace classical{


typedef std::vector<Coords> container_type;
typedef std::pair< container_type , container_type > state_type;
typedef boost::numeric::odeint::hamiltonian_stepper_rk<container_type> stepper_type_symp;
typedef boost::numeric::odeint::stepper_rk5_ck<Coords> stepper_type_rk;
typedef boost::numeric::odeint::controlled_stepper_standard< stepper_type_rk > controller;
typedef boost::numeric::odeint::controlled_stepper_bs< Coords > controller2;


class DerivClassP {
    private:
        boost::shared_ptr<Force> x_, dx_;
        bool time_;
    public:
        DerivClassP(){};

        DerivClassP(boost::shared_ptr<Force> x, bool time = false):x_(x), time_(time){};

        void operator()(const container_type &pq, container_type &dpqdt) const {

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
        }
};

class DerivClassRK {
    private:
        boost::shared_ptr<Force> kin_, pot_;

    public:
        DerivClassRK(){};

        DerivClassRK(boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot):kin_(kin), pot_(pot){};

        void operator()(const Coords &pq, Coords &dpqdt, double t) const {

            std::size_t half = pq.size()/2;
            Coords x(half), p(half);
            for (std::size_t i=0;i<half;++i){
                x[i] = pq[i];
            }

            pot_->operator()(x, p, t);
            for (std::size_t i=0;i<half;++i){
                dpqdt[i+half] = p[i];
                dpqdt[i] = pq[i+half];
            }
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
        void operator()(double t, Coords &x, DerivClassRK &px){
            LOG(INFO) << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << " at:  " << t << std::endl;
        }
};

Cpair OdeIntRKStrategy::operator()(Cpair &xin){

    DerivClassRK derivs(kin_,pot_);
    stepper_type_rk stepper;
    controller2 control(eps_abs_, eps_rel_, 1.0, 1.0);
    obs observ;

    if (observe_){
        boost::numeric::odeint::integrate_adaptive(control, derivs, xin.first, tin_, xin.second, dt_, observ);
    } else {
        boost::numeric::odeint::integrate_adaptive(control, derivs, xin.first, tin_, xin.second, dt_);
    }

    return xin;
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

    int steps = xin.second/dt_;
    for (int i=1;i<=steps;++i){
        t+=dt_;
        stepper.do_step(derivs, state, t, dt_);
    }
    //TODO: this should probably be changed to make sure that EXACTLY tfinal is done EVERYTIME. Right now we miss on a small fraction <dt
    tin_ = t;

    Coords xfin(xin.first.size());
    for (std::size_t i=0;i<half;++i){
        xfin[i] = state.first[0][i];
        xfin[i+half] = state.second[0][i];
    }

    return std::make_pair(xfin,t);
}

Integrator::Integrator(anyMap &params){

    using boost::any_cast;
    double tin, dt, eps_abs, eps_rel;
    boost::shared_ptr<Force> pot,kin;
    boost::shared_ptr<Field> dpot;
    int strat = any_cast<int>(params["int-type"]);
    bool observe;
    try{
        observe = any_cast<bool>(params["watch"]);
    } catch (boost::bad_any_cast &e) {
        observe = false;
    }

    switch(strat){
        case 1:
            intStrat_ = boost::shared_ptr<VoidIntStrategy>(new VoidIntStrategy());
            LOG(INFO) << "using void integrator, integrator will return the coordinate set it is passed without integrating";
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
            intStrat_ = boost::shared_ptr<OdeIntRKStrategy>(new OdeIntRKStrategy(tin, dt, kin, pot, eps_abs, eps_rel, observe));
            LOG(INFO) << "using odeint RK integrator";
            break;
        default:
            LOG(FATAL) << "bad choice for integrator type, exiting...";
    }
}

}
