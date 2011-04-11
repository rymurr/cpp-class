
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
//#include <boost/function.hpp>
//#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include "customGlog.hpp"
#include "coords.hpp"
#include "fields.hpp"

namespace classical{

#define pi M_PI
class Potential;

typedef std::map<std::string,boost::any> anyMap;
typedef boost::shared_ptr<std::vector<boost::shared_ptr<Potential> > > vPots;

class Potential{
    public:
        virtual ~Potential(){};
        virtual double operator()(const Coords<double>&, const double t = 0) = 0;
        static boost::shared_ptr<Potential> makePotential(anyMap&);
        static boost::shared_ptr<Potential> makePotential(boost::shared_ptr<Field> field);
        static boost::shared_ptr<Potential> makePotential(vPots pots);
};

class HAtomPotential: public Potential{
    private:
        double charge_, alpha_;

    public:
        HAtomPotential(const double charge,const double alpha): charge_(charge), alpha_(alpha*alpha){};

        virtual double operator()(const Coords<double> &r, const double t = 0){return -charge_/sqrt(square(r)+alpha_);};
};

class HMolPotential: public Potential {
    private:
        Coords<double> r1_;
        double alpha_, q1_, q2_;

    public:
        HMolPotential(const Coords<double> &r1, const double alpha, const double q1, const double q2): r1_(r1), alpha_(alpha*alpha), q1_(q1), q2_(q2){};
        virtual double operator()(const Coords<double> &r, const double t = 0){
            Coords<double> rleft = r-r1_;
            Coords<double> rright = r+r1_;
            return -q1_/sqrt(square(rleft)+alpha_)-q2_/sqrt(square(rright)+alpha_);
        };
};

class CombinedPotential: public Potential{
    private:
        vPots pots_;
    public:
        CombinedPotential(vPots pots):pots_(pots){};

        virtual double operator()(const Coords<double> &r, const double t = 0){
            double tot(0);
            std::for_each(pots_->begin(),pots_->end(),tot+=boost::lambda::bind(&Potential::operator(),*boost::lambda::_1,r,t));
            return tot;
        }
};
        
class FieldPotential: public Potential{
    private:
        boost::shared_ptr<Field> field_;
    public:
        FieldPotential(boost::shared_ptr<Field> field): field_(field){};
        virtual double operator()(const Coords<double> &r, const double t = 0){
            //TODO: VERY IMPORTANT!!! this should be fixed to calculate with proper polarization on fields
            Coords<double> x(3,0.);
            x[field_->pol()] = field_->operator()(t);
            return x.dotProd(r);
        }
};

/*
class KineticPotential: public Potential{
    pr
};
*/

inline boost::shared_ptr<Potential> Potential::makePotential(boost::shared_ptr<Field> field){
    return boost::shared_ptr<FieldPotential>(new FieldPotential(field));
};

inline boost::shared_ptr<Potential> Potential::makePotential(vPots pots){
    return boost::shared_ptr<CombinedPotential>(new CombinedPotential(pots));
};

inline boost::shared_ptr<Potential> potentialFactory(anyMap &pMap){return Potential::makePotential(pMap);};
inline boost::shared_ptr<Potential> potentialFactory(vPots pots){return Potential::makePotential(pots);};
inline boost::shared_ptr<Potential> potentialFactory(boost::shared_ptr<Field> field){return Potential::makePotential(field);};


}
#endif
