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
typedef int (*pointer_to_func)(double, const double[], double[], void*);
typedef int (*ptr_to_func)(double, const double[], double[], double[], void*);

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

class GSLForceFunc{
    private:
        boost::shared_ptr<Force> pot_, kin_;
        int size_, half_;
    public:
        GSLForceFunc(){};
        GSLForceFunc(int size, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot): pot_(pot), kin_(kin), size_(size), half_(size/2){};
        int operator()(double t, const double y[], double f[], void* parms);
};

class GSLIntStrategy: public IntStrategy{
    private:
        int dims_;
        double tin_, dt_, abs_error_, rel_error_;
        const gsl_odeiv_step_type * T;
        gsl_odeiv_step * s;
        gsl_odeiv_control * c;
        gsl_odeiv_evolve * e;
        gsl_odeiv_system sys;
        boost::shared_ptr<GSLForceFunc> gslFunc_;
        boost::function<int (double, const double[], double[], double[], void*)> jac_;
        boost::function<int (double, const double[], double[], void*)> func_;
        std::vector<boost::shared_ptr<Coords> > Cptr_;

    public:
        GSLIntStrategy(int dims, double abs_error, double rel_error, double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot): dims_(dims), tin_(tin), dt_(dt), abs_error_(abs_error),rel_error_(rel_error){
            T = gsl_odeiv_step_rk8pd;
            s = gsl_odeiv_step_alloc (T, dims_);
            c = gsl_odeiv_control_y_new (abs_error_, rel_error_);
            e = gsl_odeiv_evolve_alloc (dims_);
            gslFunc_ = boost::shared_ptr<GSLForceFunc>(new GSLForceFunc(dims_, kin, pot));

            func_ = boost::bind(&GSLForceFunc::operator(),*gslFunc_,_1,_2,_3,_4);
            jac_ = NULL;

            gsl_odeiv_system sys1 = {*func_.target<pointer_to_func>(),*jac_.target<ptr_to_func>(), dims_, &Cptr_};
            sys = sys1;
        };

        virtual ~GSLIntStrategy(){
            gsl_odeiv_evolve_free (e);
            gsl_odeiv_control_free (c);
            gsl_odeiv_step_free (s);
        };

        virtual Cpair operator()(Cpair& x);
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
