#ifndef _INTEGRATOR_HPP_
#define _INTEGRATOR_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <string>
#include <map>
#include <cmath>

#include "coords.hpp"
#include "force.hpp"
#include "customGlog.hpp"
#include "exceptions.hpp"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>

namespace classical{

typedef std::map<std::string,boost::any> anyMap;
typedef std::pair<Coords, double> Cpair;

class IntStrategy {
    public:
        virtual ~IntStrategy(){};

        virtual Cpair operator()(Cpair&) = 0;

};

class VoidIntStrategy: public IntStrategy{
    public:
        VoidIntStrategy(){};

        virtual ~VoidIntStrategy(){};

        virtual Cpair operator()(Cpair& x){return x;};
};

class GSLIntStrategy: public IntStrategy{
    private:
        double tin_, dims_, dt_;
        gsl_odeiv_step_type * T;
        gsl_odeiv_step * s;
        gsl_odeiv_control * c;
        gsl_odeiv_evolve * e;
        gsl_odeiv_system sys;

    public:
        GSLIntStrategy(int dims, double tin, double dt):tin_(tin), dims_(dims), dt_(dt){
            *T = gsl_odeiv_step_rk8pd;
            *s = gsl_odeiv_step_alloc (T, dims);
            *c = gsl_odeiv_control_y_new (1e-6, 0.0);
            *e = gsl_odeiv_evolve_alloc (dims);

            sys = {func, func, dims_, func};
        };

        virtual ~GSLIntStrategy(){
            gsl_odeiv_evolve_free (e);
            gsl_odeiv_control_free (c);
            gsl_odeiv_step_free (s);
        };

        virtual Cpair operator()(Cpair& x){return x;};
};

class SimplecticIntStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Force> kin_, pot_;

    public:
        SimplecticIntStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot):tin_(tin), dt_(dt), kin_(kin), pot_(pot){};

        virtual ~SimplecticIntStrategy(){};

        virtual Cpair operator()(Cpair&);
};

class Integrator {
    private:
        boost::shared_ptr<IntStrategy> intStrat_;

    public:
        Integrator(anyMap&);

        Cpair operator()(Cpair x){return intStrat_->operator()(x);};

};


}


#endif
