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
	        ("ndim", po::value<int>()->default_value(2),"The Number of dimensions to be used in the calculation")
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
    notify(vm);

    if (vm.count("help")){
        std::cerr << visible << "\n";
        return;
    }

    for (po::variables_map::iterator iter = vm.begin(); iter != vm.end(); ++iter){
        retMap[iter->first.c_str()] = iter->second.value();
    }

}


void print_cfg(std::string, const anyMap retMap){
	std::ofstream fp_out;
	try{
		fp_out.open("run_params.cfg");
	}
	catch(std::ofstream::failure e){
		std::cerr << "could not open the file" << std::endl;
		return;
	}

	ptime now = second_clock::local_time();
    fp_out << "#This is the set of parameters used in the previous run, performed at: "
        << to_simple_string(now)
        << std::endl;
    foreach(pairm m, retMap)
    {
        fp_out << m.first << " = " ;
        try
        {
            fp_out << any_cast<double>(m.second) <<"\n";
        }
        catch(boost::bad_any_cast & e)
        {
            try
            {
                fp_out << any_cast<int>(m.second) <<"\n";
            }
            catch(boost::bad_any_cast & e)
            {
                fp_out << any_cast<std::string>(m.second) <<"\n";
            }
        }
    }
    fp_out << std::endl;
    fp_out.close();
}

void print_cli(const anyMap retMap){

	ptime now = second_clock::local_time();
    std::cout << "This is the set of parameters being used in the current run, performed at: "
        << to_simple_string(now)
        << std::endl;
    foreach(pairm m, retMap)
    {
        std::cout << "The value of " << m.first << " is: " ;
        try
        {
             std::cout << any_cast<double>(m.second) <<"\n";
        }
        catch(boost::bad_any_cast & e)
        {
            try
            {
                std::cout << any_cast<int>(m.second) <<"\n";
            }
            catch(boost::bad_any_cast & e)
            {
                std::cout << any_cast<std::string>(m.second) <<"\n";
            }
        }
    }
    std::cout << std::endl;
}

