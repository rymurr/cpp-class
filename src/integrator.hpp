#ifndef _INTEGRATOR_HPP_
#define _INTEGRATOR_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <string>
#include <map>
#include <cmath>

#include "coords.hpp"
#include "potential.hpp"
#include "customGlog.hpp"

typedef std::map<std::string,boost::any> anyMap;

namespace classical{

class IntStrategy {
    public:
        virtual ~IntStrategy(){};

        virtual Coords<double> operator()(Coords<double>&, double) = 0;

};

class VoidIntStrategy: public IntStrategy{
    public:
        VoidIntStrategy(){};

        virtual ~VoidIntStrategy(){};

        virtual Coords<double> operator()(Coords<double>& x, double tfin){return x;};
};

class GSLIntStrategy: public IntStrategy{
    public:
        GSLIntStrategy(){};

        virtual ~GSLIntStrategy(){};

        virtual Coords<double> operator()(Coords<double>& x, double t){return Coords<double>(x);};
};

class SimplecticIntStrategy: public IntStrategy{
    private:
        double tin_, dt_;
        boost::shared_ptr<Potential> kin_, pot_;

    public:
        SimplecticIntStrategy(double tin, double dt, boost::shared_ptr<Potential> kin, boost::shared_ptr<Potential> pot):tin_(tin), dt_(dt), kin_(kin), pot_(pot){};

        virtual ~SimplecticIntStrategy(){};

        virtual Coords<double> operator()(Coords<double>&, double);
};

class Integrator {
    private:
        boost::shared_ptr<IntStrategy> intStrat_;

    public:
        Integrator(anyMap&);

        Coords<double> operator()(Coords<double> &x, double tfin){return intStrat_->operator()(x,tfin);};

};


}


#endif
