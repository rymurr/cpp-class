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

#include "coords.hpp"
#include "customGlog.hpp"
#include "exceptions.hpp"


namespace classical{

typedef std::map<std::string,boost::any> anyMap;

class WeightFunc {
    public:
        virtual ~WeightFunc(){};

        virtual double operator()(Coords& ics) =0;
};

class UnitWeight: public WeightFunc {
    public:
        UnitWeight(anyMap*){};

        virtual double operator()(Coords& ics){return 1.0;};
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
        std::vector<double> sigmas_;

    public:
        AtomWeight(anyMap* params){
            using boost::any_cast;
            sigmas_ = *any_cast<boost::shared_ptr<std::vector<double> > >((*params)["sigmas"]);
            std::vector<double> ef = any_cast<std::vector<double> >((*params)["ef"]);
            double init=0;
            double efs = std::accumulate(ef.begin(),ef.end(),init,std::plus<double>());
            double ip(any_cast<double>((*params)["ip"]));
            atom_ = atomProb(efs, ip);
            //std::cout << ef[0] << ip << atom_ << efs<<std::endl;
        }

        virtual double operator()(Coords &ics){
            double retVal = atom_;
            for (std::size_t i=0;i<sigmas_.size();++i){
                retVal *= gaussProb(ics[i],sigmas_[i]);
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

        WeightGen(anyMap* params){
            using boost::any_cast;
            int id(any_cast<int>((*params)["weight-func"]));
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

        virtual double operator()(Coords& ics){return func_ ->operator()(ics);};
};

}
//need to do more here for molecules and more sophisticated atomic weights. Lots more sophistication can be done
//also may do frozen gaussian amps here
#endif
