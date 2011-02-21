#ifndef IC_WEIGHTS_HPP
#define IC_WEIGHTS_HPP

#include <map>
#include <string>
#include <iostream>
#include <fstream>
//#include <iterator>
#include <stdexcept>
#include <vector>
//#include <ctime>
//#include <functional>
//#include <numeric>
//#include <typeinfo>
//#include <gsl/gsl_math.h>
#include <math.h>

#include <boost/any.hpp>
//#include <boost/cast.hpp>
//#include <boost/function.hpp>
//#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/shared_ptr.hpp>

#include "customGlog.hpp"



typedef std::map<std::string,boost::any> anyMap;
typedef boost::shared_ptr<std::vector<double> > vTraj;

inline double unitWeight(anyMap&, vTraj){
    return 1.0;
};

inline double gaussProb(double arg, double tau){
    return exp(-arg*arg*tau);
}

inline double atomProb(double ef, double ip){
    return exp(-2.*sqrt(ip*ip*ip)/(3.*ef));
}

inline double atomWeight(anyMap &params, vTraj ics){
    using boost::any_cast;
    //typedef std::vector<double> dVec;
    vTraj sigmas(any_cast<vTraj>(params["sigmas"]));
    double retVal(atomProb(any_cast<double>(params["ef"]),any_cast<double>(params["ip"])));
    for (std::size_t i=0;i<sigmas->size();++i){
        retVal *= gaussProb((*ics)[i],(*sigmas)[i]);
    }
    return retVal;
}

//need to do more here for molecules and more sophisticated atomic weights. Lots more sophistication can be done
//also may do frozen gaussian amps here
#endif
