
#ifndef POTS_H_
#define POTS_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <typeinfo>
#include <gsl/gsl_math.h>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "customGlog.hpp"
#include "coords.hpp"

namespace classical{

#define pi M_PI

typedef std::map<std::string,boost::any> anyMap;

class Potential{
    public:
        virtual ~Potential(){};
        virtual double operator()(Coords&) = 0;
        static boost::shared_ptr<Potential> makePotential(anyMap);
};

class HAtomPotential: public Potential{
    private:
        double charge_, alpha_;

    public:
        HAtomPotential(double charge,double alpha): charge_(charge), alpha_(alpha*alpha){};

        double operator()(Coords &r){return -charge_/sqrt(square(r)+alpha_);};
};

class HMolPotential: public Potential {
    private:
        Coords r1_;
        double alpha_, q1_, q2_;

    public:
        HMolPotential(Coords &r1, double alpha, double q1, double q2): r1_(r1), alpha_(alpha*alpha), q1_(q1), q2_(q2){};
        double operator()(Coords &r){
            Coords rleft = r-r1_;
            Coords rright = r+r1_;
            return -q1_/sqrt(square(rleft)+alpha_)-q2_/sqrt(square(rright)+alpha_);
        };
};

inline boost::shared_ptr<Potential> potentialFactory(anyMap pMap){return Potential::makePotential(pMap);}

}
#endif
