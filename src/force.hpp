
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
#include "potential.hpp"
#include "fields.hpp"

namespace classical{

#define pi M_PI
class Force;

typedef std::map<std::string,boost::any> anyMap;
typedef boost::shared_ptr<std::vector<boost::shared_ptr<Force> > > vForce;

class Force{
    public:
        virtual ~Force(){};
        virtual void operator()(const Coords&, Coords&, const double t = 0) const = 0;
        static boost::shared_ptr<Force> makeForce(anyMap&);
        static boost::shared_ptr<Force> makeForce(boost::shared_ptr<Field> field);
        static boost::shared_ptr<Force> makeForce(vForce &pots, int n);
        static boost::shared_ptr<Force> makeForce(int n);
        static boost::shared_ptr<Force> makeForce(boost::shared_ptr<Potential> pot);
};

class HAtomForce: public Force{
    private:
        double charge_, alpha_;

    public:
        HAtomForce(const double charge,const double alpha): charge_(charge), alpha_(alpha*alpha){};

        virtual void operator()(const Coords &r, Coords &x, const double t = 0) const {double norm=square(r);x = -charge_*r/sqrt(norm+alpha_);};
};

class HMolForce: public Force {
    private:
        Coords r1_;
        double alpha_, q1_, q2_;

    public:
        HMolForce(const Coords &r1, const double alpha, const double q1, const double q2): r1_(r1), alpha_(alpha*alpha), q1_(q1), q2_(q2){};
        virtual void operator()(const Coords &r, Coords &x, const double t = 0) const {
            double rleft = square(r-r1_);
            double rright = square(r+r1_);
            x = -q1_*(r-r1_)/sqrt(rleft*rleft+alpha_)-q2_*(r+r1_)/sqrt(rright*rright+alpha_);
        };
};

class CombinedForce: public Force{
    private:
        vForce pots_;
        int n_;
    public:
        CombinedForce(vForce pots, int n):pots_(pots),n_(n){};

        virtual void operator()(const Coords &r, Coords &x, const double t = 0) const{

            for(std::size_t i=0;i<pots_->size();++i){
                Coords temp(n_,0);
                pots_->operator [](i)->operator ()(r, temp, t);
                x += temp;
            }
        }
};

class FieldForce: public Force{
    private:
        boost::shared_ptr<Field> field_;
        int pol_;
    public:
        FieldForce(boost::shared_ptr<Field> field): field_(field), pol_(field_->pol()-1){};
        virtual void operator()(const Coords &r, Coords &x, const double t = 0) const {
            x = Coords(r.size(),0.);x[pol_] = field_->operator()(t);
        }
        void deriv(const Coords &r, Coords &x, const double t = 0) const {
            x = Coords(r.size(),0.);x[pol_] = r[pol_] * field_->deriv(t);
        }
};

class PotentialForce: public Force{
    private:
        boost::shared_ptr<Potential> pot_;
    public:
        PotentialForce(boost::shared_ptr<Potential> pot): pot_(pot){};
        virtual void operator()(const Coords &r, Coords &x, const double t = 0) const {
            x[0] = pot_->operator()(r, t);
        };
};

class NullForce: public Force{
    public:
        virtual void operator()(const Coords &r, Coords &x, const double t=0) const {x = Coords(r.size(),0);};
};

class KineticForce: public Force{
    public:
        virtual void operator()(const Coords &r, Coords &x, const double t=0) const {x = r;};
};

inline boost::shared_ptr<Force> Force::makeForce(boost::shared_ptr<Potential> pot){
    return boost::shared_ptr<PotentialForce>(new PotentialForce(pot));
};

inline boost::shared_ptr<Force> Force::makeForce(boost::shared_ptr<Field> field){
    return boost::shared_ptr<FieldForce>(new FieldForce(field));
};

inline boost::shared_ptr<Force> Force::makeForce(vForce &pots, int n){
    return boost::shared_ptr<CombinedForce>(new CombinedForce(pots, n));
};

inline boost::shared_ptr<Force> Force::makeForce(int n){
    boost::shared_ptr<Force> fpick;
    switch (n){
        case 5:
            fpick = boost::shared_ptr<NullForce>(new NullForce());
            LOG(INFO) << "null force is being used";
            break;
        case 6:
            fpick = boost::shared_ptr<KineticForce>(new KineticForce());
            LOG(INFO) << "using force variant for kinetic energy operator";
            break;
        default:
            LOG(FATAL) << "the choice of potential" + boost::lexical_cast<std::string>(n) + "is not valid, the code will exit now";
    }
    return fpick;
}

inline boost::shared_ptr<Force> forceFactory(anyMap &pMap){return Force::makeForce(pMap);};
inline boost::shared_ptr<Force> forceFactory(vForce pots, int n){return Force::makeForce(pots, n);};
inline boost::shared_ptr<Force> forceFactory(boost::shared_ptr<Field> field){return Force::makeForce(field);};
inline boost::shared_ptr<Force> forceFactory(boost::shared_ptr<Potential> pot){return Force::makeForce(pot);};
inline boost::shared_ptr<Force> forceFactory(int n){return Force::makeForce(n);};

}
#endif
