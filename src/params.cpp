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

pairm validate(pairm& m, boost::any newVal){
    if (m.second.type() == typeid(intRunPtr)){
        any_cast<intRunPtr>(m.second)->set(any_cast<int>(newVal));
        any_cast<intRunPtr>(m.second)->verify();
        pairm retVal (m.first,newVal);
        return retVal;
    } else if (m.second.type() == typeid(intListPtr)){
        any_cast<intListPtr>(m.second)->set(any_cast<std::string>(newVal));
        any_cast<intListPtr>(m.second)->verify();
        std::vector<int> retArray = *(any_cast<intListPtr>(m.second)->pArray);
        pairm retVal (m.first,retArray);
        return retVal;
    } else if (m.second.type() == typeid(doubleListPtr)){
        any_cast<doubleListPtr>(m.second)->set(any_cast<std::string>(newVal));
        any_cast<doubleListPtr>(m.second)->verify();
        std::vector<double> retArray = *(any_cast<doubleListPtr>(m.second)->pArray);
        pairm retVal (m.first,retArray);
        return retVal;
    } else if (m.second.type() == typeid(statePtr)){
        any_cast<statePtr>(m.second)->set(any_cast<int>(newVal));
        any_cast<statePtr>(m.second)->verify();
        pairm retVal (m.first,newVal);
        return retVal;
    } else if (m.second.type() == typeid(doubleRunPtr)){
        any_cast<doubleRunPtr>(m.second)->set(any_cast<double>(newVal));
        any_cast<doubleRunPtr>(m.second)->verify();
        pairm retVal (m.first,newVal);
        return retVal;
    } else {
        throw std::invalid_argument("wrong data in parameter array " + m.first + " in validate");
    }
}

void gen_param(pairm m, po::options_description& desc){

    if (m.second.type() == typeid(intRunPtr)){
        desc.add_options()
            (any_cast<intRunPtr>(m.second)->name->c_str(),po::value<int>()->default_value(*(any_cast<intRunPtr>(m.second)->defaultValue)),any_cast<intRunPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(intListPtr)){
        desc.add_options()
            (any_cast<intListPtr>(m.second)->name->c_str(),po::value<std::string>()->default_value(any_cast<intListPtr>(m.second)->defaultValue->c_str()),any_cast<intListPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(doubleListPtr)){
        desc.add_options()
            (any_cast<doubleListPtr>(m.second)->name->c_str(),po::value<std::string>()->default_value(any_cast<doubleListPtr>(m.second)->defaultValue->c_str()),any_cast<doubleListPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(doubleRunPtr)){
        desc.add_options()
            (any_cast<doubleRunPtr>(m.second)->name->c_str(),po::value<double>()->default_value(*(any_cast<doubleRunPtr>(m.second)->defaultValue)),any_cast<doubleRunPtr>(m.second)->description->c_str());
        return;
    } else if (m.second.type() == typeid(statePtr)){
        desc.add_options()
            (any_cast<statePtr>(m.second)->name->c_str(),po::value<int>()->default_value(*(any_cast<statePtr>(m.second)->defaultValue)),any_cast<statePtr>(m.second)->description->c_str());
        return;
    } else {
        throw std::invalid_argument("wrong data in parameter array " + m.first + " in gen_param");
    }
}

pmap::pmap(){
    
}

pmap::~pmap(){

}

void pmap::read_params(std::string fname, int ac, char** av){

    anyMap general_opts, time_opts, wf_opts, field_opts;
    std::vector<anyMap> opts;
    opts.push_back(general_opts);
    opts.push_back(time_opts);
    opts.push_back(wf_opts);
    opts.push_back(field_opts);

    boost::shared_ptr<int> ndim_def(new int), charges_def(new int), field_def(new int);
    defMap dist_map, traj_map, inwf_map, inpot_map, field_map;
    boost::assign::insert(dist_map)
        (1,"Monte-Carlo")
        (2,"linear")
        (3,"staged-linear (not implemented)")
        ;
    boost::assign::insert(traj_map)
        (1,"Check parameters only")
        (2,"Build ICs only")
        (3,"run trajectories only")
        (4,"run binning only")
        (5,"everything")
        ;
    boost::assign::insert(inwf_map)
        (1,"Hydrogen atom like")
        (2,"Hydrogen molecule like")
        (3,"GAMESS checkpoint file")
        (4,"anything on a grid")
        ;
    boost::assign::insert(inpot_map)
        (1,"Hydrogen atom like")
        (2,"Hydrogen molecule like")
        (3,"GAMESS checkpoint file")
        (4,"anything on a grid")
        ;
    boost::assign::insert(field_map)
        (1,"constant cosine field")
        (2,"static field")
        (3,"gaussian envelope")
        (4,"sine squard envelope")
        (5,"numerical form")
        ;

    general_opts["ndim"] = intRunPtr(new intRun("The Number of dimensions to be used in the calculation", "ndim", 2, 100, 1));
    general_opts["dims"] = intListPtr(new intList("The number of trajectories in each dimension. comma separated list.", "dims", "1,1",ndim_def));
    general_opts["dist-type"] = statePtr(new state_param("The number of trajectories in each dimension. comma separated list.", "dist-type", 1, dist_map));
    general_opts["run-type"] = statePtr(new state_param("Type of run", "run-type", 5, traj_map));
    general_opts["id-stages"] = intRunPtr(new intRun("Number of stages for staged-linear distribution(not implemented)", "id-stages", 1, 100, 1));

    time_opts["tinitial"] = doubleRunPtr(new doubleRun("Start time of the simulation, can be negative", "tinitial", 0, 10E10, -10E10));
    time_opts["tfinal"] = doubleRunPtr(new doubleRun("Finish time of the simulation, can be negative", "tfinal", 100, 10E10, -10E10));
    time_opts["dt"] = doubleRunPtr(new doubleRun("Initial time step", "dt", 0.01, 10E10, -10E10));
    

    wf_opts["wf-type"] = statePtr(new state_param("Type of initial wave-function", "wf-type", 1, inwf_map));
    wf_opts["pot-type"] = statePtr(new state_param("Type of initial potential", "pot-type", 1, inpot_map));
    wf_opts["charges"] = doubleListPtr(new doubleList("charge on core(s). list of numbers for each core only needed for H-like and H2-like potential. 1 is for singly ionized etc.", "charges", "1",charges_def));
    wf_opts["smoothing"] = doubleRunPtr(new doubleRun("Smoothing parameter to avoid coulomb singularity. Should be small and is only needed when using analytic potentials.", "smoothing", 1E-4, 10E10, -10E10));
    wf_opts["rnuc"] = doubleRunPtr(new doubleRun("Internuclear separation in atomic units, only needed for analytic potentials", "rnuc", 0.0, 10E10, -10E10));
    wf_opts["theta-nuc"] = doubleRunPtr(new doubleRun("alignment of molecule wrt the laser polarization. 0 is aligned, 90 is anti-aligned. given in degrees.", "theta-nuc", 0.0, 10E10, -10E10));
    wf_opts["ip"] = doubleRunPtr(new doubleRun("ionization potential of target, given in atomic units", "ip", 0.5, 10E10, -10E10));
    wf_opts["state"] = doubleRunPtr(new doubleRun("state of atomic or molecular system (not implemented)", "state", 0.01, 10E10, -10E10));

    field_opts["nfield"] = intRunPtr(new intRun("Number of fields, every option below is given as a comma separated list of options for each field. Currently only 1 field is allowed", "ndim", 2, 100, 1));
    field_opts["env"] = statePtr(new state_param("Type of fields", "env", 1,field_map));
    field_opts["ef"] = doubleListPtr(new doubleList("Field strengths given in atomic units", "ef", "1",field_def));
    field_opts["omega"] = doubleListPtr(new doubleList("frequencies given in atomic units", "omega", "1",field_def));
    field_opts["fwhm"] = doubleListPtr(new doubleList("FWHM of envelope, ignored for static or constant field.", "fwhm", "1",field_def));
    field_opts["ce"] = doubleListPtr(new doubleList("initial CE phase of each field given in degrees", "ce", "1",field_def));

    po::options_description generic("Generic Options");
    po::options_description timepos("Time Grid");
    po::options_description wfopts("Wave function and potential");
    po::options_description field("Field options");
    generic.add_options()
	        ("help", "displays help message");

    //TODO: combine this into one step!!
    foreach(pairm m, general_opts){
        gen_param(m, generic);
    }

    foreach(pairm m, time_opts){
        gen_param(m, timepos);
    }

    foreach(pairm m, wf_opts){
        gen_param(m, wfopts);
    }

    foreach(pairm m, field_opts){
        gen_param(m, field);
    }

    po::options_description cmdline;
	    cmdline.add(generic).add(timepos);

    po::options_description config;
        config.add(generic).add(timepos);

    po::options_description visible("Allowed options");
        visible.add(generic).add(timepos);

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

    for (po::variables_map::iterator iter = vm.begin(); iter != vm.end(); ++iter){
        map[iter->first.c_str()] = iter->second.value();
    }

    *ndim_def = any_cast<int>(map["ndim"]);
    *charges_def = any_cast<int>(map["charges"]);
    *field_def = any_cast<int>(map["nfield"]);

    foreach(pairm m, general_opts){
            boost::any newVal = map[m.first];
            pairm n = validate(m,newVal);
            map[n.first] = n.second;
    }
    foreach(pairm m, time_opts){
            boost::any newVal = map[m.first];
            pairm n = validate(m,newVal);
            map[n.first] = n.second;
    }
    foreach(pairm m, wf_opts){
            boost::any newVal = map[m.first];
            pairm n = validate(m,newVal);
            map[n.first] = n.second;
    }
    foreach(pairm m, field_opts){
            boost::any newVal = map[m.first];
            pairm n = validate(m,newVal);
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
                    try{
                        foreach(int i, any_cast<std::vector<int> >(m.second)){
                            std::cout << i << " ";
                        }
                        std::cout << "\n";
                    }
                    catch(boost::bad_any_cast & e){
                        std::cout << "n/a" << "\n";
                    }
                }   
            }
        }
    }
    std::cout << std::endl;
}
