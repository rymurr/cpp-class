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
#include "exceptions.hpp"


namespace classical{

typedef std::map<std::string,boost::any> anyMap;
typedef boost::shared_ptr<std::vector<double> > vTraj;

class WeightFunc {
    public:
        virtual ~WeightFunc(){};

        virtual double operator()(vTraj ics) =0;
};

class UnitWeight: public WeightFunc {
    public:
        UnitWeight(anyMap&){};

        double operator()(vTraj ics){return 1.0;};
};

inline double gaussProb(double arg, double tau){
    return exp(-arg*arg*tau);
}

inline double atomProb(double ef, double ip){
    return exp(-2.*sqrt(ip*ip*ip)/(3.*ef));
}

class AtomWeight: public WeightFunc {
    private:
        double atom_;
        vTraj sigmas_;

    public:
        AtomWeight(anyMap &params){
            using boost::any_cast;
            sigmas_ = any_cast<vTraj>(params["sigmas"]);
            double ef(any_cast<double>(params["ef"]));
            double ip(any_cast<double>(params["ip"]));
            atom_ = atomProb(ef, ip);
        }

        double operator()(vTraj ics){
            double retVal = atom_;
            for (std::size_t i=0;i<sigmas_->size();++i){
                retVal *= gaussProb((*ics)[i],(*sigmas_)[i]);
            }
            return retVal;
        }
};

typedef boost::shared_ptr<WeightFunc> wFun;

class WeightGen {
    private:
        wFun func_;
    public:
        WeightGen(){};

        ~WeightGen(){};

        WeightGen(anyMap params){
            using boost::any_cast;
            int id(any_cast<int>(params["weight-func"]));
            DLOG(INFO) << "building WeightGen object";
            switch(id){
                case 1:
                    func_ = boost::shared_ptr<UnitWeight>(new UnitWeight(params));
                    break;
                case 2:
                    func_ = boost::shared_ptr<AtomWeight>(new AtomWeight(params));
                    break;
                default:
                    LOG(ERROR) << "was unable to create WeightFunc object! wrong id";
                    throw invalid_parameter() << err_info("unable to create WeightFunc due to an invalid choice for weight func");
            }
        };

        virtual double operator()(vTraj ics){return func_ ->operator()(ics);};
};

}
//need to do more here for molecules and more sophisticated atomic weights. Lots more sophistication can be done
//also may do frozen gaussian amps here
#endif
