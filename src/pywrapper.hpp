#ifndef PYWRAPPER_HPP_
#define PYWRAPPER_HPP_

#include <Python.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include <boost/lambda/lambda.hpp>
#include <boost/python.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/list.hpp>


#include "input_param.hpp"
#include "icgen.hpp"
#include "fields.hpp"
//#include "potential.hpp"
#include "zeros.hpp"
#include "coords.hpp"


namespace classical{

class Potential;
//class Field;

using namespace boost::python;

class simulation{
    private:
        int argc_;
        std::vector<std::string> argv_;
        std::string fname_;
        pmap params_;
        boost::shared_ptr<anyMap> map_;
        std::vector<boost::shared_ptr<Field> > fieldVec_;
        boost::shared_ptr<std::vector<boost::shared_ptr<Potential> > > pots_;
        boost::shared_ptr<Potential> totPot_;
        boost::shared_ptr<icgenerator> icgen_;

        void common_setup(){
#ifdef GFlags_FOUND
            google::InitGoogleLogging("cpp-class");
            google::InstallFailureSignalHandler();
#endif
        };

    public:
        simulation(int argc, boost::python::list argv, std::string fname): argc_(argc), fname_(fname){
            std::size_t n = boost::python::len(argv);
            for (unsigned int i = 0; i < n; i++) {
                argv_.push_back(boost::python::extract<std::string>(argv[i]));
            }
            common_setup();
        };

        simulation(int argc, std::vector<std::string> argv, std::string fname): argc_(argc), argv_(argv), fname_(fname){
            common_setup();
        };

        ~simulation(){
#ifdef GFlags_FOUND
            google::ShutdownGoogleLogging();
#endif
        };

        void param_init(){
            LOG(INFO) << "building parameter set from input data";
            if (!params_.read_params(fname_, argc_, argv_)){LOG(ERROR) << "Parameters build was unsuccessful";return;};
            params_.print();
            map_ = params_.map_out();
            LOG(INFO) << "Parameters have been built";
        };

        void ic_gen();

        void run_trajs();
};


int run_main(int argc, std::vector<std::string> argv, std::string fname);

}

#endif
