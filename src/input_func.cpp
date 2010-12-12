/*
 * input_func.cpp
 *
 *  Created on: Dec 12, 2010
 *      Author: ryanmurray
 */

#include "input_func.hpp"

void params_in(int ac, char** av, anyMap &retMap){

    po::options_description generic("Generic Options");
	    generic.add_options()
	        ("help", "displays help message")
	        ("nthreads", po::value<int>()->default_value(1),"The number of fftw threads to be used")
	        ("printwf", po::value<int>()->default_value(1),"which wf to print for results \n 1. print psix final \n 2. psix initial \n 3. psip initial \n 4. psip final")
	        ;

    po::options_description timepos("Time and Space Grids");
	    timepos.add_options()
	        ("tmax", po::value<double>()->default_value(2), "maximum value of T in laser periods. if omega=0 then max value of T")
	        ("dt", po::value<double>()->default_value(0.05), "time step in atomic units")
	        ;

    po::options_description cmdline;
	    cmdline.add(generic).add(timepos);

    po::options_description config;
        config.add(generic).add(timepos);

    po::options_description visible("Allowed options");
        visible.add(generic).add(timepos);

    po::variables_map vm;
        po::store(po::parse_command_line(ac, av, cmdline), vm);
        po::notify(vm);

    std::string fname = any_cast<std::string>(retMap["ConfigFile"]);


    std::ifstream ifs(fname.c_str());

        store(parse_config_file(ifs, config), vm);
        std::cout << fname.c_str() <<std::endl;
        notify(vm);


    if (vm.count("help")){
        std::cerr << visible << "\n";
        return;
    }

    for (po::variables_map::iterator iter = vm.begin(); iter != vm.end(); ++iter){
        retMap[iter->first.c_str()] = iter->second.value();
    }

}
