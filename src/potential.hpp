
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
//#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "customGlog.hpp"

namespace classical{

#define pi M_PI

typedef std::map<std::string,boost::any> anyMap;

class potential{
    private:
        double z1_, x1_, y1_, alpha_;
        std::vector<double> charges_;

        boost::function<double (double,double,double)> fpick;

        double hatom(double, double, double);

        double hmol(double, double, double);

    public:
        potential();

        potential(anyMap);

        ~potential();

        double operator()(double, double, double);

};

inline double potential::operator()(double x, double y, double z){
    return (*this).fpick(x,y,z);
}

inline double potential::hatom(double x, double y, double z){
    return -charges_[0]/sqrt(x*x+y*y+z*z+alpha_*alpha_);
}

inline double potential::hmol(double x, double y, double z){
    return  -charges_[0]/sqrt((x-x1_)*(x-x1_)+(y-y1_)*(y-y1_)+(z-z1_)*(z-z1_)+alpha_*alpha_)-charges_[1]/sqrt((x+x1_)*(x+x1_)+(y+y1_)*(y+y1_)+(z+z1_)*(z+z1_)+alpha_*alpha_);
}
}
#endif
