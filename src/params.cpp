// $Id$
/**
 * @file params.cpp
 * the source for the params functions. This function reads the command line and
 * from a config file and stores the values in a map to be passed back to the 
 * controlling program
 *
 * @brief definition of wavefunction class
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$

#include "params.hpp"

pairm validate(pairm m){
    if (m.second.type() == typeid(run_param<int>)){
        pairm retVal (m.first,(any_cast<run_param<int> >(m.second)).actual);
        (any_cast<run_param<int> >(m.second)).verify();
        return retVal;
    } else {
        throw std::invalid_argument("wrong data in parameter array");
    }
}

void gen_param(pairm m, po::options_description& desc){
    //TODO: create lambda function here to keep from writing many copies of the same thing
    //TODO: overload equal operator in param class to make less writing.

    if (m.second.type() == typeid(run_param<int>)){
        desc.add_options()
            ((any_cast<run_param<int> >(m.second)).name.c_str(),po::value<int>()->default_value((any_cast<run_param<int> >(m.second)).defaultValue),(any_cast<run_param<int> >(m.second)).description.c_str());
            //((any_cast<run_param<int> >(m.second)).name,po::value<int>()->default_value((any_cast<run_param<int> >(m.second)).defaultValue),(any_cast<run_param<int> >(m.second)).description);
        return;
    } else {
        throw std::invalid_argument("wrong data in parameter array " + m.first);
    }
}

pmap::pmap(){
    
    run_param<int> ndim("The Number of dimensions to be used in the calculation", "General", "ndim", 1, 1, 100, 0);
    //TODO: can this be automated?
    general_opts[ndim.name] = ndim;
    
}

pmap::~pmap(){

}

void pmap::read_params(std::string fname, int ac, char** av){
    po::options_description generic("Generic Options");
    generic.add_options()
	        ("help", "displays help message");

    foreach(pairm m, general_opts){
        gen_param(m, generic);
    }

    po::options_description cmdline;
	    cmdline.add(generic);

    po::options_description config;
        config.add(generic);

    po::options_description visible("Allowed options");
        visible.add(generic);

    po::variables_map vm;
        po::store(po::parse_command_line(ac, av, cmdline), vm);
        po::notify(vm);

    std::ifstream ifs(fname.c_str());
    store(parse_config_file(ifs, config), vm);
    notify(vm);

    if (vm.count("help")){
        std::cerr << visible << "\n";
        return;
    }

//TODO: make sure maps of params classes are updated to new values here!!!


}

void pmap::validate_map(){
    //TODO: the actual value needs to be updated before this will work1!!
    foreach(pairm m, general_opts){
        pairm n = validate(m);
        map[n.first] = n.second;
    }
}

void pmap::print(std::string runParams){
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
    foreach(pairm m, map)
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

void pmap::print(){

	ptime now = second_clock::local_time();
    std::cout << "This is the set of parameters being used in the current run, performed at: "
        << to_simple_string(now)
        << std::endl;
    foreach(pairm m, map)
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
