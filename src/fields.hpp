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

#include "input_param.hpp"

#define pi M_PI

using boost::any_cast;


class field {
    
    public:
        field(pmap map, int);

        field();

        virtual ~field();

        double operator()(double);

    private:
        double omega, fwhm, ef, phi;

        boost::function<double (double)> fpick;

        double fconst(double);

        double fstatic(double);

        double gaussian(double);

        double ssquare(double);

};
#endif

