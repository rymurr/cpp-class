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

#include <glog/logging.h>


//#define pi M_PI

typedef std::map<std::string,boost::any> anyMap;
typedef boost::multi_array<double,1> w_array;
typedef boost::multi_array<double,2> ic_array;
typedef boost::shared_ptr<std::vector<double> > vTraj;

// This is a typedef for a random number generator.
// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
//try boost::rand48 for quickest/least accurate
typedef boost::minstd_rand base_generator_type;

class SingleIC{
       

    public:

        int size_;

        double var_;

        SingleIC();

        ~SingleIC();

        SingleIC(double, int);

        void virtual RetVal(vTraj){};

        double virtual RetVal(){return 0;};

        void virtual reset(){};

};

class SingleLinIC: public SingleIC{
    private:

        double mean_, current_, dx_, finish_, start_;

    public:

        SingleLinIC();

        ~SingleLinIC();

        SingleLinIC(double, double, int);

        double RetVal();
        
        void RetVal(vTraj){};

        void reset();
 };

class SingleRandIC: public SingleIC{
    private:
        std::vector<double> means_;
        boost::normal_distribution<> dist_;
        boost::shared_ptr<boost::variate_generator<base_generator_type&, boost::normal_distribution<> > >  generators_;
        base_generator_type generator_;
    public:
        SingleRandIC();

        ~SingleRandIC();

        SingleRandIC(std::vector<double>, double);

        void RetVal(vTraj);

        double RetVal(){return 0;};
};

class icgenerator{

    private:
        int tType_, tnumb_, j_;
        std::vector<double> means_;
        std::vector<int> trajs_,tsing_;
        double variance_;
        ic_array initConditions_;
        w_array weights_;
        boost::function<void (void)> fpick;
        boost::function<void (vTraj)> gpick, hpick;
        bool single_, lindone_;
        std::vector<boost::shared_ptr<SingleIC> > gens_;

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
            ar >> tType_;
            ar >> tnumb_;
            ar & means_;
            ar & trajs_;
            ar & tsing_;
            ar & variance_;
            ar & single_;
            ar & lindone_;
//            ar & weights_;
            int tot=1, max; 

            if (single_ == false){
                LOG(INFO) << "the icgenerator will build a full set of ICs and store them for later use";
                switch (tType_){
                    case 1:
                        fpick = boost::bind(&icgenerator::montecarlo,this);
                        std::for_each(trajs_.begin(),trajs_.end(),tot*=boost::lambda::_1);
                        initConditions_.resize(boost::extents[trajs_.size()][tot]);
                        break;
                    case 2:
                        fpick = boost::bind(&icgenerator::linear,this);
                        max = *(std::max_element(trajs_.begin(),trajs_.end()));
                        initConditions_.resize(boost::extents[trajs_.size()][max]);    
                        break;
                    case 3:
                        LOG(FATAL) << "staged-linear is currently not implemented";
                    default:
                        LOG(FATAL) << "Bad choice for Initial conditions";
                }
                for (unsigned int i=0;i<initConditions_.shape()[0];i++){
                    for (unsigned int j=0;j<initConditions_.shape()[1];j++){
                        ar & initConditions_[i][j];
                    }
                }
            } else {
                LOG(INFO) << "The icgenerator is generating ICs on the fly and will not store them";
                switch (tType_){
                    case 1:
                        gens_.push_back(boost::shared_ptr<SingleRandIC>(new SingleRandIC(means_,variance_)));
                        gpick = boost::bind(&icgenerator::singlemc,this,_1);
                        break;
                    case 2:
                        for(unsigned int i=0;i<trajs_.size();i++){
                            gens_.push_back(boost::shared_ptr<SingleLinIC>(new SingleLinIC(means_[i],variance_,trajs_[i])));
//                            tsing_.push_back(0);
                        }
//                        lindone_ = false;
                        gpick = boost::bind(&icgenerator::singlelin,this,_1);
                        break;
                     case 3:
                        LOG(FATAL) << "staged-linear is currently not implemented";
                    default:
                        LOG(FATAL) << "Bad choice for Initial conditions";
                }
            }

        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()
        
        void montecarlo();
        
        void singlemc(vTraj);

        void mcsingle(vTraj);

        void linear();

        void singlelin(vTraj);

        void linsingle(vTraj);

        void gen_ics();

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

#endif
