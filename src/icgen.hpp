#ifndef ICGEN_H_
#define ICGEN_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
//#include <iterator>
//#include <stdexcept>
#include <vector>
#include <ctime>
#include <functional>
#include <numeric>
//#include <typeinfo>
//#include <gsl/gsl_math.h>

#include <boost/any.hpp>
//#include <boost/cast.hpp>
//#include <boost/array.hpp>
#include <boost/multi_array.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>
#include <boost/progress.hpp>
#include <boost/foreach.hpp>

#include "customGlog.hpp"

#include "exceptions.hpp"
#include "ic_single.hpp"

//#define pi M_PI


namespace classical{

#define foreach     BOOST_FOREACH

typedef std::map<std::string,boost::any> anyMap;
typedef std::vector<double> w_array;
typedef boost::multi_array<double,2> ic_array;
//typedef boost::shared_ptr<std::vector<double> > vTraj;
typedef boost::multi_array_types::index_range range;
//typedef boost::function<double (anyMap, vTraj)> wFun;

using boost::any_cast;

class icgenerator{

    private:
        int tType_, tnumb_, tdim_, j_, k_;
        std::vector<double> means_;
        double variance_;
        std::vector<int> trajs_;
        ic_array initConditions_;
        w_array weights_;
        bool single_;
        boost::shared_ptr<SingleIC> icGens_;
        boost::shared_ptr<WeightGen> wGens_;

    friend class boost::serialization::access;
        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar & tType_;
            ar & tnumb_;
            ar & tdim_;
            ar & means_;
            ar & trajs_;
            ar & variance_;
            ar & single_;
            ar & j_;
            ar & k_;
            if (!single_){
                for (unsigned int i=0;i<initConditions_.shape()[0];i++){
                    for (unsigned int j=0;j<initConditions_.shape()[1];j++){
                        ar & initConditions_[i][j];
                    }
                }
                foreach(double x, weights_){
                    ar & x;
                }
            }
        }
        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar & tType_;
            ar & tnumb_;
            ar & tdim_;
            ar & means_;
            ar & trajs_;
            ar & variance_;
            ar & single_;
            ar & j_;
            ar & k_;

            (*this).singleCheck();
            if (!single_){
            	for (unsigned int i=0;i<initConditions_.shape()[0];i++){
                    for (unsigned int j=0;j<initConditions_.shape()[1];j++){
                        ar & initConditions_[i][j];
                    }
            	}
            	foreach(double x, weights_){
            	    ar & x;
            	}
            }

        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
        
        void genICs();
        
        void singleCheck();

    public:
        icgenerator(){};

        icgenerator(anyMap params, anyMap wParams=anyMap());

        ~icgenerator(){};

        //TODO: currently the pointer to a private member is being returned
        //while quite bad i am doing this to avoid a LARGE copy
        //i may change this later
        void retICs(boost::shared_ptr<ic_array> icPtr){*icPtr = initConditions_;};

        void retWeights(boost::shared_ptr<w_array> wPtr){*wPtr = weights_;};

        void retIC(vTraj ics){
            if (j_ < tnumb_){
                if (single_) {
                    icGens_->operator()(ics);
                } else {
                    std::transform(initConditions_[j_].begin(),initConditions_[j_].end(),ics->begin(),boost::lambda::_1);
                }
                ++j_;
            } else {
                throw out_of_trajectories() << err_info("Reached the end of the trajectories");
            }
        };

        double retWeight(vTraj ics){
            if (k_ < tnumb_){
                if (single_) {
                    return wGens_->operator()(ics);
                } else {
                    return weights_[k_];
                }
                ++k_;
            } else {
                throw out_of_trajectories() << err_info("Reached the end of the trajectories");
            }
        };

        void setWeights(anyMap);

        void save(std::string, std::string);

        void load(std::string sType, std::string fName, anyMap params=anyMap());
};

}
#endif
