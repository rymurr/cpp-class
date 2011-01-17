#ifndef ICGEN_H_
#define ICGEN_H_

#include <map>
#include <string>
#include <iostream>
//#include <fstream>
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

        double var_;

        int size_;

        SingleIC();

        ~SingleIC();

        SingleIC(double, int);

        void virtual RetVal(vTraj){};

        double virtual RetVal(){return 0;};

        void virtual reset(){};
        
};

class SingleLinIC: public SingleIC{
    private:

        double current_, dx_, mean_, finish_, start_;

    public:

        SingleLinIC();

        ~SingleLinIC();

        SingleLinIC(double, double, int);

        double RetVal();

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
};

class icgenerator{

    private:
        int tType_, tnumb_;
        std::vector<double> means_;
        std::vector<int> trajs_,tsing_;
        double variance_;
        ic_array initConditions_;
        w_array weights_;
        boost::function<void (void)> fpick;
        boost::function<void (vTraj)> gpick;
        bool single_,lindone_;
        std::vector<boost::shared_ptr<SingleIC> > gens_;

    friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & means_;
            ar & trajs_;
            ar & tsing_;
            ar & variance_;
            ar & single_;
            ar & lindone_;
            ar & fpick;
            ar & gpick;
            ar & initConditions_;
            ar & weights_;
            ar & gens_;
        }
        
        void montecarlo();
        
        void singlemc(vTraj);

        void linear();

        void singlelin(vTraj);

    public:
        icgenerator();

        icgenerator(anyMap);

        ~icgenerator();

        //TODO: currently the pointer to a private member is being returned
        //while quite bad i am doing this to avoid a LARGE copy
        //i may change this later
        void ret_ics(boost::shared_ptr<ic_array>);

        void ret_weight(boost::shared_ptr<w_array>);

        void gen_ics();

        void get_ic(vTraj);

        void gen_weights(boost::function<void (anyMap)>, anyMap);
};

#endif
