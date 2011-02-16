#ifndef ICGEN_H_
#define ICGEN_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
//#include <iterator>
#include <stdexcept>
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
#include <boost/shared_ptr.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>
#include <boost/progress.hpp>

#include <glog/logging.h>


//#define pi M_PI

namespace classical{


typedef std::map<std::string,boost::any> anyMap;
typedef boost::multi_array<double,1> w_array;
typedef boost::multi_array<double,2> ic_array;
typedef boost::shared_ptr<std::vector<double> > vTraj;
typedef boost::multi_array_types::index_range range;

// This is a typedef for a random number generator.
// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
//try boost::rand48 for quickest/least accurate
typedef boost::minstd_rand base_generator_type;

class SingleIC{
       

    public:
        int size_;

        double var_;

        std::vector<double> mean_;

        SingleIC();

        ~SingleIC();

        SingleIC(std::vector<double>, double);

        void virtual RetVal(vTraj){};

        void virtual reset(int){};

};

class SingleLinIC: public SingleIC{
    private:

        std::vector<double> current_, dx_, finish_, start_;
        std::vector<int> sizes_;

    public:

        SingleLinIC();

        ~SingleLinIC();

        SingleLinIC(std::vector<double>, double, std::vector<int>);
        
        void RetVal(vTraj);

//        void reset();
 };

class SingleRandIC: public SingleIC{
    private:
        //std::vector<double> means_;
        boost::normal_distribution<> dist_;
        boost::shared_ptr<boost::variate_generator<base_generator_type&, boost::normal_distribution<> > >  generators_;
        base_generator_type generator_;
    public:
        SingleRandIC();

        ~SingleRandIC();

        SingleRandIC(std::vector<double>, double);

        void RetVal(vTraj);
};

class icgenerator{

    private:
        int tType_, tnumb_, tdim_, j_;
        std::vector<double> means_;
        double variance_;
        std::vector<int> trajs_,tsing_;
        ic_array initConditions_;
        w_array weights_;
        //boost::function<void (void)> fpick;
        boost::function<void (vTraj)> gpick, hpick;
        bool single_, lindone_;
        boost::shared_ptr<SingleIC> gens_;
        std::vector<ic_array::array_view<1>::type> sliceVec_;
        std::vector<boost::multi_array<double,1>::iterator> sliceIter_;

    friend class boost::serialization::access;
        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar & tType_;
            ar & tnumb_;
            ar & means_;
            ar & trajs_;
            ar & tsing_;
            ar & variance_;
            ar & single_;
            ar & lindone_;
            ar & j_;
            for (unsigned int i=0;i<initConditions_.shape()[0];i++){
                for (unsigned int j=0;j<initConditions_.shape()[1];j++){
                    ar & initConditions_[i][j];
                }
            }
//            ar & weights_;
        }
        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar & tType_;
            ar & tnumb_;
            ar & means_;
            ar & trajs_;
            ar & tsing_;
            ar & variance_;
            ar & single_;
            ar & lindone_;
            ar & j_;
//            ar & weights_;

            (*this).singleCheck();
            if (single_ == false){
            	for (unsigned int i=0;i<initConditions_.shape()[0];i++){
                    for (unsigned int j=0;j<initConditions_.shape()[1];j++){
                        ar & initConditions_[i][j];
                    }
            	}
            }

        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
        
        void montecarloFill();
        
        void singleMCFill(vTraj);

        void singleMCRet(vTraj);

        void linearFill();

        void singleLinFill(vTraj);

        void singleLinRet(vTraj);

        //void gen_ics();

        void tTypeSwitch();

        void singleCheck();

    public:
        icgenerator();

        icgenerator(anyMap);

        ~icgenerator();

        //TODO: currently the pointer to a private member is being returned
        //while quite bad i am doing this to avoid a LARGE copy
        //i may change this later
        void ret_ics(boost::shared_ptr<ic_array>);

        void ret_weight(boost::shared_ptr<w_array>);

        void get_ic(vTraj);

        void gen_weights(boost::function<void (anyMap)>, anyMap);

        void save(std::string, std::string);

        void load(std::string, std::string);
};

}
#endif
