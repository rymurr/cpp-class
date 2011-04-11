
#ifndef FORCE_H_
#define FORCE_H_

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
class Force;

typedef std::map<std::string,boost::any> anyMap;
typedef boost::shared_ptr<std::vector<boost::shared_ptr<Force> > > vForce;

class Force{
    public:
        virtual ~Force(){};
        virtual Coords<double> operator()(const Coords<double>&, const double t = 0) = 0;
        static boost::shared_ptr<Force> makeForce(anyMap&);
        static boost::shared_ptr<Force> makeForce(boost::shared_ptr<Field> field);
        static boost::shared_ptr<Force> makeForce(vForce pots, int n);
};

class HAtomForce: public Force{
    private:
        double charge_, alpha_;

    public:
        HAtomForce(const double charge,const double alpha): charge_(charge), alpha_(alpha*alpha){};

        virtual Coords<double> operator()(const Coords<double> &r, const double t = 0){return -charge_*r/sqrt(square(r)+alpha_);};
};

class HMolForce: public Force {
    private:
        Coords<double> r1_;
        double alpha_, q1_, q2_;

    public:
        HMolForce(const Coords<double> &r1, const double alpha, const double q1, const double q2): r1_(r1), alpha_(alpha*alpha), q1_(q1), q2_(q2){};
        virtual Coords<double> operator()(const Coords<double> &r, const double t = 0){
            Coords<double> rleft = r-r1_;
            Coords<double> rright = r+r1_;
            return -q1_*rleft/sqrt(square(rleft)+alpha_)-q2_*rright/sqrt(square(rright)+alpha_);
        };
};

class CombinedForce: public Force{
    private:
        vForce pots_;
        int n_;
    public:
        CombinedForce(vForce pots, int n):pots_(pots),n_(n){};

        virtual Coords<double> operator()(const Coords<double> &r, const double t = 0){
            Coords<double> tot(n_,0);
//            for(int i=pots_->begin();i=!pots_->end();++i){
//                tot+= i->operator()(r,t);
//            }
            return tot;
        }
};

class FieldForce: public Force{
    private:
        boost::shared_ptr<Field> field_;
    public:
        FieldForce(boost::shared_ptr<Field> field): field_(field){};
        virtual Coords<double> operator()(const Coords<double> &r, const double t = 0){
            //TODO: VERY IMPORTANT!!! this should be fixed to calculate with proper polarization on fields
            Coords<double> x(3,0.);
            x[field_->pol()] = field_->operator()(t);
            return x;
        }
};

/*
class KineticPotential: public Potential{
    pr
};
*/

inline boost::shared_ptr<Force> Force::makeForce(boost::shared_ptr<Field> field){
    return boost::shared_ptr<FieldForce>(new FieldForce(field));
};

inline boost::shared_ptr<Force> Force::makeForce(vForce pots, int n){
    return boost::shared_ptr<CombinedForce>(new CombinedForce(pots, n));
};

inline boost::shared_ptr<Force> forceFactory(anyMap &pMap){return Force::makeForce(pMap);};
inline boost::shared_ptr<Force> forceFactory(vForce pots, int n){return Force::makeForce(pots, n);};
inline boost::shared_ptr<Force> forceFactory(boost::shared_ptr<Field> field){return Force::makeForce(field);};


}
#endif
