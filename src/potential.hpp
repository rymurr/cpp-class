
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
        virtual double operator()(const Coords&, const double t = 0) = 0;
        static boost::shared_ptr<Potential> makePotential(anyMap&);
        static boost::shared_ptr<Potential> makePotential(boost::shared_ptr<Field> field);
        static boost::shared_ptr<Potential> makePotential(vPots pots);
        static boost::shared_ptr<Potential> makePotential();
};

class HAtomPotential: public Potential{
    private:
        double charge_, alpha_;

    public:
        HAtomPotential(const double charge,const double alpha): charge_(charge), alpha_(alpha*alpha){};

        virtual double operator()(const Coords &r, const double t = 0){using boost::numeric::ublas::norm_2;double norm=norm_2(r);return -charge_/sqrt(norm*norm+alpha_);};
};

class HMolPotential: public Potential {
    private:
        Coords r1_;
        double alpha_, q1_, q2_;

    public:
        HMolPotential(const Coords &r1, const double alpha, const double q1, const double q2): r1_(r1), alpha_(alpha*alpha), q1_(q1), q2_(q2){};
        virtual double operator()(const Coords &r, const double t = 0){
            using boost::numeric::ublas::norm_2;
            double rleft = norm_2(r-r1_);
            double rright = norm_2(r+r1_);
            return -q1_/sqrt(rleft*rleft+alpha_)-q2_/sqrt(rright*rright+alpha_);
        };
};

class CombinedPotential: public Potential{
    private:
        vPots pots_;
    public:
        CombinedPotential(vPots pots):pots_(pots){};

        virtual double operator()(const Coords &r, const double t = 0){
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
        virtual double operator()(const Coords &r, const double t = 0){
            //using boost::numeric::ublas::inner_prod;
            //TODO: VERY IMPORTANT!!! this should be fixed to calculate with proper polarization on fields
            //Coords x(3,0.);
            return r[field_->pol()-1] * field_->operator()(t);
            //return inner_prod(x,r);
        }
};

class KineticPotential: public Potential{
    public:
        virtual double operator()(const Coords &r, const double t = 0){
            using boost::numeric::ublas::inner_prod;
            return 0.5 * inner_prod(r,r);
        };
};


inline boost::shared_ptr<Potential> Potential::makePotential(boost::shared_ptr<Field> field){
    return boost::shared_ptr<FieldPotential>(new FieldPotential(field));
};

inline boost::shared_ptr<Potential> Potential::makePotential(vPots pots){
    return boost::shared_ptr<CombinedPotential>(new CombinedPotential(pots));
};

inline boost::shared_ptr<Potential> Potential::makePotential(){
    return boost::shared_ptr<KineticPotential>(new KineticPotential());
};

inline boost::shared_ptr<Potential> potentialFactory(anyMap &pMap){return Potential::makePotential(pMap);};
inline boost::shared_ptr<Potential> potentialFactory(vPots pots){return Potential::makePotential(pots);};
inline boost::shared_ptr<Potential> potentialFactory(boost::shared_ptr<Field> field){return Potential::makePotential(field);};
inline boost::shared_ptr<Potential> potentialFactory(){return Potential::makePotential();};

}
#endif
