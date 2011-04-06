#ifndef FIELDS_H_
#define FIELDS_H_

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

#include "customGlog.hpp"


namespace classical{

#define pi M_PI

typedef std::map<std::string,boost::any> anyMap;

class Field {
    
    public:

        virtual ~Field(){};

        virtual double operator()(double) = 0;

        static boost::shared_ptr<Field> makeField(anyMap params, int id);

};

class ConstField: public Field {
    private:
        double ef_, omega_, phi_;
        int id_;

    public:
        ConstField(int id, double ef, double omega, double phi):ef_(ef), omega_(omega), phi_(phi), id_(id){};

        double operator()(double t){return ef_ * cos(omega_*t+phi_);};
};

class StaticField: public Field {
    private:
        double ef_;
        int id_;

    public:
        StaticField(int id, double ef):ef_(ef) ,id_(id){};

        double operator()(double t){return ef_;};
};

class GaussianField: public Field {
    private:
        double ef_, omega_, phi_, tmid_, fwhm_;
        int id_;

    public:
        GaussianField(int id, double ef, double omega, double phi, double tmid, double fwhm):ef_(ef), omega_(omega), phi_(phi), tmid_(tmid), id_(id){
            fwhm_ = -4.0*log(2.0)/(fwhm*fwhm);
        };

        double operator()(double t){return ef_ * cos(omega_*t+phi_) * exp(-fwhm_*(t-tmid_)*(t-tmid_));};
};

class SineSquareField: public Field {
    private:
        double ef_, omega_, phi_, fwhm_;
        int id_;

    public:
        SineSquareField(int id, double ef, double omega, double phi, double fwhm):ef_(ef), omega_(omega), phi_(phi), id_(id){
            fwhm_ = pi*0.5/fwhm;
        };

        double operator()(double t){return ef_ * cos(omega_*t+phi_) * pow(sin(fwhm_*t),2);};
};

inline boost::shared_ptr<Field> fieldFactory(anyMap pMap, int id){return Field::makeField(pMap,id);}

}
#endif

