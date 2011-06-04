#ifndef _INTEGRATOR_HPP_
#define _INTEGRATOR_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <string>
#include <map>
#include <cmath>

#include "boost/numeric/odeint.hpp"

#include "coords.hpp"
#include "force.hpp"
#include "customGlog.hpp"
#include "exceptions.hpp"

namespace classical{

typedef std::vector<Coords> container_type;
typedef std::pair< container_type , container_type > state_type;
typedef boost::numeric::odeint::hamiltonian_stepper_rk<container_type> stepper_type_symp;
typedef boost::numeric::odeint::stepper_rk5_ck<Coords> stepper_type_rk;
typedef boost::numeric::odeint::controlled_stepper_standard< stepper_type_rk > controller;
typedef boost::numeric::odeint::controlled_stepper_bs< Coords > controller2;

typedef std::map<std::string,boost::any> anyMap;
typedef std::pair<Coords, double> Cpair;
typedef int (*pointer_to_func)(double, const double[], double[], void*);
typedef int (*ptr_to_func)(double, const double[], double[], double[], void*);

static double escapeRange;


class checkTrap {
    private:
        bool check_;
    public:
        checkTrap(bool check):check_(check){};

        bool operator()(Coords x){
            if (!check_) return true;
            if (abs(x)<escapeRange){
                return false;
            } else {
                return true;
            }
        };

        bool operator()(state_type state){
            if (!check_) return true;
            Coords x(state.first[0].size());
            for (std::size_t i=0;i<x.size();++i){
                x[i] = state.first[0][i];
            }
            return this->operator()(x);
        };
};


class IntStrategy {
    public:
        virtual ~IntStrategy(){};

        virtual Cpair operator()(const Cpair&) = 0;

};

class VoidIntStrategy: public IntStrategy{
    public:
        VoidIntStrategy(){};

        virtual ~VoidIntStrategy(){};

        virtual Cpair operator()(const Cpair& x){return x;};
};

class OdeIntRKStrategy: public IntStrategy{
    private:
        double tin_, dt_, tCheck_;
        boost::shared_ptr<Force> kin_, pot_;
        double eps_abs_, eps_rel_;
        bool observe_;
        checkTrap dCheck_;

    public:
        OdeIntRKStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot, double eps_abs, double eps_rel, bool observe, double tCheck=0, checkTrap dCheck = checkTrap(false)): tin_(tin), dt_(dt), tCheck_(tCheck), kin_(kin), pot_(pot), eps_abs_(eps_abs), eps_rel_(eps_rel), observe_(observe), dCheck_(checkTrap(dCheck)){};
        virtual ~OdeIntRKStrategy(){};
        virtual Cpair operator()(const Cpair &x);
};

class OdeIntSympStrategy: public IntStrategy{
    private:
        double tin_, dt_, tCheck_;
        boost::shared_ptr<Force> kin_, pot_;
        boost::shared_ptr<Field> dpot_;
        checkTrap dCheck_;

    public:
        OdeIntSympStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot, boost::shared_ptr<Field> dpot = boost::shared_ptr<Field>(), double tCheck=0, checkTrap dCheck = checkTrap(false)): tin_(tin), dt_(dt), tCheck_(tCheck), kin_(kin), pot_(pot), dpot_(dpot), dCheck_(checkTrap(dCheck)){};
        virtual ~OdeIntSympStrategy(){};
        virtual Cpair operator()(const Cpair& x);
};

class Integrator {
    private:
        boost::shared_ptr<IntStrategy> intStrat_;

    public:
        Integrator(anyMap&);
        Integrator(){};
        Cpair operator()(const Cpair x){return intStrat_->operator()(x);};

};


}


#endif
