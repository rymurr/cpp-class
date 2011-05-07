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

//#include <gsl/gsl_errno.h>
//#include <gsl/gsl_matrix.h>
//#include <gsl/gsl_odeiv.h>

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

class OdeIntRKStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Force> kin_, pot_;
        double eps_abs_, eps_rel_;
        bool observe_;

    public:
        OdeIntRKStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot, double eps_abs, double eps_rel, bool observe): tin_(tin), dt_(dt), kin_(kin), pot_(pot), eps_abs_(eps_abs), eps_rel_(eps_rel), observe_(observe){};
        virtual ~OdeIntRKStrategy(){};
        virtual Cpair operator()(Cpair &x);
};

class OdeIntSympStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Force> kin_, pot_;
        boost::shared_ptr<Field> dpot_;

    public:
        OdeIntSympStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot, boost::shared_ptr<Field> dpot = boost::shared_ptr<Field>()): tin_(tin), dt_(dt), kin_(kin), pot_(pot), dpot_(dpot){};
        virtual ~OdeIntSympStrategy(){};
        virtual Cpair operator()(Cpair& x);
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
