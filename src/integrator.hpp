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

//#include "boost/numeric/odeint.hpp"

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

class SymplecticIntStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Force> kin_, pot_;

    public:
        SymplecticIntStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot):tin_(tin), dt_(dt), kin_(kin), pot_(pot){};

        virtual ~SymplecticIntStrategy(){};

        virtual Cpair operator()(Cpair&);
};

class OdeIntRKStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Force> kin_, pot_;

    public:
        OdeIntRKStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot): tin_(tin), dt_(dt), kin_(kin), pot_(pot){};
        virtual ~OdeIntRKStrategy(){};
        virtual Cpair operator()(Cpair &x){return x;};
};

class OdeIntSympStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Force> kin_, pot_;

    public:
        OdeIntSympStrategy(double tin, double dt, boost::shared_ptr<Force> kin, boost::shared_ptr<Force> pot): tin_(tin), dt_(dt), kin_(kin), pot_(pot){};
        virtual ~OdeIntSympStrategy(){};
        virtual Cpair operator()(Cpair& x){return x;};
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
