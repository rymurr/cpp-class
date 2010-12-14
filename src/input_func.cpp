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
	        ("ndim,n", po::value<int>()->default_value(2),"The Number of dimensions to be used in the calculation")
	        ("dims", po::value<std::string>()->default_value("1,1"),"The number of trajectories in each dimension. comma separated list.")
	        ("dist-type", po::value<int>()->default_value(1),"Type of distribution:\n 1. monte-carlo \n 2. linear \n 3. staged-linear")
            ("id-stages", po::value<int>()->default_value(1), "Number of stages for staged-linear distribution(not implemented)")
            ("traj-type,t", po::value<int>()->default_value(2), "Type of trajectory run, choices: \n 1. frozen gaussian(not implemented) \n 2. classical \n 3. FGA and classical(not implemented) ")

            ("run-type,r", po::value<int>()->default_value(5), "Type of run, choices: \n 1. read params only \n 2. build ICs only \n 3. run trajectories only \n 4. run binning only \n 5. everything \n 6. combinations to be defined later, currently we only run the whole wad.")
	        ;

    po::options_description timepos("Time Grid");
	    timepos.add_options()
            
            ("tinitial", po::value<double>()->default_value(0), "Start time of the simulation, can be negative")
            ("tfinal", po::value<double>()->default_value(100), "Finish time of the simulation, in atomic units")
            ("dt", po::value<double>()->default_value(0.05), "initial time step.")
            
	        ;

    po::options_description wfopts("Wave function and potential");
        wfopts.add_options()
            ("init_ft,w", po::value<int>()->default_value(1), "type of initial wave-function: \n 1. H like \n 2. H2+ like \n 3. GAMESS checkpoint \n 4. anything on a grid(see docs)")        
            ("init_pot,p", po::value<int>()->default_value(1), "type of initial wave-function: \n 1. H like \n 2. H2+ like \n 3. GAMESS checkpoint \n 4. anything on a grid(see docs)")
            ("charges", po::value<std::string>()->default_value("1"), "charge on core(s). list of numbers for each core only needed for H-like and H2-like potential. 1 is for singly ionized etc.")
            ("smoothing", po::value<double>()->default_value(0.05), "Smoothing parameter to avoid coulomb singularity. Should be small and is only needed when using analytic potentials.")
            ("rnuc", po::value<double>()->default_value(0), "Internuclear separation in atomic units, only needed for analytic potentials")
            ("theta-nuc", po::value<double>()->default_value(0), "alignment of molecule wrt the laser polarization. 0 is aligned, 90 is anti-aligned. given in degrees.")
            ("ip", po::value<double>()->default_value(0.5), "ionization potential of target, given in atomic units")
            ("state", po::value<std::string>()->default_value("0"), "state of atomic or molecular system (not implemented)")
            ;

    po::options_description field("Electric field options");
        field.add_options()
            ("nfield", po::value<int>()->default_value(1), "Number of fields, every option below is given as a comma separated list of options for each field. Currently only 1 field is allowed")
            ("envelope", po::value<std::string>()->default_value("1"), "Type of fields: \n 1. constant cosine field \n 2. static field \n 3. gaussian envelope \n 4. sine squared envelope \n 5. numerical from file (not implemented)")
            ("strength", po::value<std::string>()->default_value("0.1"), "Field strengths given in atomic units")
            ("freq", po::value<std::string>()->default_value("0.057"), "frequencies given in atomic units")
            ("fwhm", po::value<std::string>()->default_value("1"), "FWHM of envelope, ignored for static or constant field.")
            ("ce-phase", po::value<std::string>()->default_value("0"), "initial CE phase of each field given in degrees")
            ;

    po::options_description runvals("Run parameters");
        runvals.add_options()
            ("relerr", po::value<double>()->default_value(1E-7), "Relative error of trajectory solver")
            ("abserr", po::value<double>()->default_value(1E-9), "Absolute error of trajectory solver")
            ("cpus", po::value<int>()->default_value(1), "number of cpus/nodes to use in an mpi run(not implemented)")
	        ("nthreads", po::value<int>()->default_value(1),"The number of threads to be used to solve trajectories in parallel")
            ("mklthreads", po::value<int>()->default_value(1), "The number of threads used to solve each trajectory, see docs")
            ("datout", po::value<std::string>()->default_value("output.dat"), "output file for data")
            ("datloc", po::value<int>()->default_value(1), "where to send data: \n 0. send to standard out \n 1. send to file")
            ("logout", po::value<std::string>()->default_value("output.log"), "log output file")
            ("logloc", po::value<int>()->default_value(0), "where to send log: \n 0. standard error console \n 1. send to file (not implemented)")
            ;

    po::options_description outputs("Output formats");
        outputs.add_options()
            ("outputs", po::value<std::string>()->default_value(""), "Output types, refer to docs. (not implemented)")
            ;
//TODO: get rid of all the not implementeds above!
//TODO: find a way to pass more info about system and run dir to code...may be needed in the future

    po::options_description cmdline;
	    cmdline.add(generic).add(timepos).add(wfopts).add(field).add(runvals).add(outputs);

    po::options_description config;
        config.add(generic).add(timepos).add(wfopts).add(field).add(runvals).add(outputs);

    po::options_description visible("Allowed options");
        visible.add(generic).add(timepos).add(wfopts).add(field).add(runvals).add(outputs);

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


void print_cfg(std::string runParams, const anyMap retMap){
	std::ofstream fp_out;
	try{
		fp_out.open(runParams.c_str());
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
                try{
                    fp_out << any_cast<std::string>(m.second) <<"\n";
                }
                catch(boost::bad_any_cast & e) {
//TODO: must fix this to print out vectors!
                    fp_out << "n/a\n";
                }   
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
                try{
                    std::cout << any_cast<std::string>(m.second) <<"\n";
                }
                catch(boost::bad_any_cast & e) {
//TODO: must fix this to print out vectors!
                    std::cout << "n/a\n";
                }   
            }
        }
    }
    std::cout << std::endl;
}

