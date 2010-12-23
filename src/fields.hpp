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

using boost::any_cast;
typedef std::map<std::string,boost::any> anyMap;

class field {
    
    public:
        field(anyMap map, int);

        field();

        virtual ~field();

        double operator()(double);

    private:
        double omega, fwhm, ef, phi, tmid;

        boost::function<double (double)> fpick;

        double fconst(double);

        double fstatic(double);

        double gaussian(double);

        double ssquare(double);

};
#endif

