#ifndef FIELDS_H
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
//#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>


#define pi M_PI

typedef std::map<std::string,boost::any> anyMap;

class field {
    
    public:
        field(anyMap map, int);

        field();

        virtual ~field();

        double operator()(double);

    private:
        int id_;

        double omega_, fwhm_, ef_, phi_, tmid_;

        boost::function<double (double)> fpick;

        double fconst(double);

        double fstatic(double);

        double gaussian(double);

        double ssquare(double);

};

inline double field::operator()(double t){
    return (*this).fpick(t);
}

inline double field::fconst(double t){
    return ef_ * cos(omega_*t+phi_);
}

inline double field::fstatic(double t){
    return ef_;
}

inline double field::gaussian(double t){
    return fconst(t) * exp(-4.0*log(2.0)*(t-tmid_)*(t-tmid_)/(fwhm_*fwhm_));
}

inline double field::ssquare(double t){
    return fconst(t) * pow(sin(pi*t*0.5/fwhm_),2);
}
#endif

