#ifndef FIELDS_H_
#define FIELDS_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <typeinfo>
#include <math.h>

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
    
    private:
        static int tot_;
        static const double h_;

    public:

        virtual ~Field(){};

        virtual double operator()(double) = 0;

        static boost::shared_ptr<Field> makeField(anyMap& params);

        virtual int pol() = 0 ;

        double deriv(double t){return (-this->operator()(t+2.*h_)+8.*this->operator()(t+h_)-8.*this->operator()(t-h_)+this->operator()(t-2.*h_))/(12.*h_);};
};

class ConstField: public Field {
    private:
        double ef_, omega_, phi_;
        int id_, pol_;

    public:
        ConstField(int id, double ef, double omega, double phi, int pol):ef_(ef), omega_(omega), phi_(phi), id_(id), pol_(pol){};

        virtual double operator()(double t){return -ef_ * cos(omega_*t+phi_);};

        virtual int pol(){return pol_;};
};

class StaticField: public Field {
    private:
        double ef_;
        int id_, pol_;

    public:
        StaticField(int id, double ef, double pol):ef_(ef) ,id_(id), pol_(pol){};

        virtual double operator()(double t){return -ef_;};

        virtual int pol(){return pol_;};
};

class GaussianField: public Field {
    private:
        double ef_, omega_, phi_, tmid_, fwhm_;
        int id_, pol_;

    public:
        GaussianField(int id, double ef, double omega, double phi, double tmid, double fwhm, double pol):ef_(ef), omega_(omega), phi_(phi), tmid_(tmid), id_(id), pol_(pol){
            fwhm_ = -4.0*log(2.0)/(fwhm*fwhm);
        };

        virtual double operator()(double t){return -ef_ * cos(omega_*t+phi_) * exp(-fwhm_*(t-tmid_)*(t-tmid_));};

        virtual int pol(){return pol_;};
};

class SineSquareField: public Field {
    private:
        double ef_, omega_, phi_, fwhm_;
        int id_, pol_;

    public:
        SineSquareField(int id, double ef, double omega, double phi, double fwhm, double pol):ef_(ef), omega_(omega), phi_(phi), id_(id), pol_(pol){
            fwhm_ = pi*0.5/fwhm;
        };

        virtual double operator()(double t){return -ef_ * cos(omega_*t+phi_) * pow(sin(fwhm_*t),2);};

        virtual int pol(){return pol_;};
};

inline boost::shared_ptr<Field> fieldFactory(anyMap& pMap){return Field::makeField(pMap);}

}
#endif

