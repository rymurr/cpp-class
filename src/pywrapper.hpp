#ifndef PYWRAPPER_HPP_
#define PYWRAPPER_HPP_

#include <string>
#include <vector>
#include <iostream>

#include <boost/lambda/lambda.hpp>

#include "input_param.hpp"
#include "fields.hpp"

namespace classical{

using namespace boost::python;

class simulation{
    private:
        int argc_;
        std::vector<std::string> argv_;
        std::string fname_;
        pmap params_;

        void common_setup(){
            google::InitGoogleLogging(argv[0].c_str());
            google::InstallFailureSignalHandler();
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
            google::ShutdownGoogleLogging();
        };

        void param_init(){
            if (!params_.read_params(fname, argc, argv)){return 0;};
            params_.print();
            LOG(INFO) << "Parameters have been built";
        };

};


int run_main(int argc, std::vector<std::string> argv, std::string fname);

}

#endif
