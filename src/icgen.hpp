#ifndef ICGEN_H_
#define ICGEN_H_

#include <map>
#include <string>
#include <iostream>
//#include <fstream>
//#include <iterator>
#include <vector>
#include <ctime>
//#include <typeinfo>
//#include <gsl/gsl_math.h>

#include <boost/any.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/array.hpp>
#include <boost/multi_array.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <glog/logging.h>


//#define pi M_PI

typedef std::map<std::string,boost::any> anyMap;
typedef boost::multi_array<double,1> w_array;
typedef boost::multi_array<double,2> ic_array;

// This is a typedef for a random number generator.
// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
//try boost::rand48 for quickest/least accurate
typedef boost::minstd_rand base_generator_type;

class icgenerator{
    private:
        int tType_;
        std::vector<double> trajs_;
        std::vector<std::pair<double,double> > ranges_;
        ic_array initConditions_;
        w_array weights_;
        boost::function<void (void)> fpick;

        void montecarlo();

        void linear();

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

        void gen_weights(boost::function<void (anyMap)>, anyMap);
};

#endif
