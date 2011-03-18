// $Id$
/**
 * @file input_param.hpp
 * This class reads in a set of parameters and stores them in a std::map
 * The map can be exported for use in the main numerical algorithm
 * boost program_options is used along with custom verify functions
 *
 * @brief params definitions
 *
 * @author Ryan Murray
 * @version 2.00
 */
// $Log$

#include "input_param.hpp"
namespace classical {

pmap::pmap(){
    
}

pmap::~pmap(){

}

bool pmap::read_params(std::string fname, int argc, std::vector<std::string> &argv){

    //TODO: need locations for input files: numerical input
    //TODO: need to add an auxilliary variable to accept user variables. Right now everything gets put to standard files
    paramVec general_opts, time_opts, wf_opts, field_opts, run_opts;
    
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
        (6,"everything from 3")
        (7,"everything from 4")
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

    general_opts.push_back(intRunPtr(new intRun("The Number of dimensions to be used in the calculation", "ndim", 2, 100, 1)));
    general_opts.push_back(intListPtr(new intList("The number of trajectories in each dimension. comma separated list.", "dims", "1,1",ndim_def)));
    general_opts.push_back(statePtr(new state_param("The number of trajectories in each dimension. comma separated list.", "dist-type", 1, dist_map)));
    general_opts.push_back(statePtr(new state_param("Type of run", "run-type", 5, traj_map)));
    general_opts.push_back(intRunPtr(new intRun("Number of stages for staged-linear distribution(not implemented)", "id-stages", 1, 100, 1)));
    general_opts.push_back(filePtr( new file_param("Input file, expects serialized results of previous run", "input_file", "in.dat")));
    general_opts.push_back(filePtr( new file_param("Output file, either serialized results of current run or results of whole run. If empty, then results will print to stdout for results of whole run.", "output_file", "out.dat")));

    time_opts.push_back(doubleRunPtr(new doubleRun("Start time of the simulation, can be negative", "tinitial", 0, 10E10, -10E10)));
    time_opts.push_back(doubleRunPtr(new doubleRun("Finish time of the simulation, can be negative", "tfinal", 100, 10E10, -10E10)));
    time_opts.push_back(doubleRunPtr(new doubleRun("Initial time step", "dt", 0.01, 10E10, -10E10)));
    
    wf_opts.push_back(statePtr(new state_param("Type of initial wave-function", "wf-type", 1, inwf_map)));
    wf_opts.push_back(statePtr(new state_param("Type of initial potential", "pot-type", 1, inpot_map)));
    wf_opts.push_back(doubleListPtr(new doubleList("charge on core(s). list of numbers for each core only needed for H-like and H2-like potential. 1 is for singly ionized etc.", "charges", "1",charges_def)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("Smoothing parameter to avoid coulomb singularity. Should be small and is only needed when using analytic potentials.", "smoothing", 1E-4, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("Internuclear separation in atomic units, only needed for analytic potentials", "rnuc", 0.0, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("alignment of molecule wrt the laser polarization. 0 is aligned, 90 is anti-aligned. given in degrees.", "theta-nuc", 0.0, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("ionization potential of target, given in atomic units", "ip", 0.5, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("state of atomic or molecular system (not implemented)", "state", 0.01, 10E10, -10E10)));
    
    field_opts.push_back(intRunPtr(new intRun("Number of fields, every option below is given as a comma separated list of options for each field. Currently only 1 field is allowed", "nfield", 1, 100, 1)));
    field_opts.push_back(statePtr(new state_param("Type of fields", "env", 1,field_map)));
    field_opts.push_back(doubleListPtr(new doubleList("Field strengths given in atomic units", "ef", "1",field_def)));
    field_opts.push_back(doubleListPtr(new doubleList("frequencies given in atomic units", "omega", "1",field_def)));
    field_opts.push_back(doubleListPtr(new doubleList("FWHM of envelope, ignored for static or constant field.", "fwhm", "1",field_def)));
    field_opts.push_back(doubleListPtr(new doubleList("initial CE phase of each field given in degrees", "ce", "1",field_def)));

    run_opts.push_back(intRunPtr(new intRun("Number of cuncurrent trajectories", "nthreads", 1, 100, 1)));
    run_opts.push_back(intRunPtr(new intRun("Number of threads per trajectory", "mklthreads", 1, 100, 1)));
    run_opts.push_back(doubleRunPtr(new doubleRun("Absolute error", "abserr", 1E-9, 10E10, -10E10)));
    run_opts.push_back(doubleRunPtr(new doubleRun("Relative error", "relerr", 1E-7, 10E10, -10E10)));

    po::options_description generic("Generic Options");
    po::options_description timepos("Time Grid");
    po::options_description wfopts("Wave function and potential");
    po::options_description field("Field options");
    po::options_description runopts("Run specific options");
    generic.add_options()
	        ("help", "displays help message");
    generic.add_options()
	        ("help-glog", "displays help messages for google glog, the logging interface");
    generic.add_options()
    		("no-build", "builds the initial conditions on the fly, without holding entire set in memory");
    generic.add_options()
            ("binary", "serialize in binary(OS-dependent), if not set serializes to text(default)");

    std::vector<paramVec> opts;
    opts.push_back(general_opts);
    opts.push_back(time_opts);
    opts.push_back(wf_opts);
    opts.push_back(field_opts);
    opts.push_back(run_opts);

    foreach(paramVec x, opts){
        foreach(paramPtr m, x){
            m->genParam(generic);
        }
    }

    po::options_description cmdline;
	    cmdline.add(generic).add(timepos).add(wfopts).add(field).add(runopts);

    po::options_description config;
        config.add(generic).add(timepos).add(wfopts).add(field).add(runopts);

    po::options_description visible("Allowed options");
        visible.add(generic).add(timepos).add(wfopts).add(field).add(runopts);

    po::variables_map vm;

    char** av=new char*[argc];
    for(int i=0;i<argc;i++){
        av[i] = &argv[i][0];
    }
    po::parsed_options parsed = po::command_line_parser(argc, av).options(cmdline).allow_unregistered().run();
    delete[] av;

    po::store(parsed, vm);
    po::notify(vm);

    std::ifstream ifs(fname.c_str());
    po::store(parse_config_file(ifs, config), vm);
    po::notify(vm);

    std::vector<std::string> to_gflags = po::collect_unrecognized(parsed.options,po::include_positional);
    std::vector<std::string>::iterator it;
    it = to_gflags.begin();
    to_gflags.insert(it,argv[0]);

    
//    delete[] av2;
    std::string usage = "Options included by google-glog for logging purposes";
#ifdef Gflags
    google::SetUsageMessage(usage);
#endif
    if (vm.count("help")){
        std::cout << visible << "\n";
        return false;
    }
    if (vm.count("help-glog")){
        to_gflags.push_back("--help");
        int gsize = to_gflags.size();
        char** av2=new char*[gsize];
        for(int i=0;i<gsize;i++){
            av2[i] = &to_gflags[i][0];
        }
#ifdef Gflags
        google::ParseCommandLineFlags(&gsize, &av2, true);
#endif
        return false; 
    } else {
        int gsize = to_gflags.size();
        char** av2=new char*[gsize];
        for(int i=0;i<gsize;i++){
            av2[i] = &to_gflags[i][0];
        }
#ifdef Gflags
        google::ParseCommandLineFlags(&gsize, &av2, true);
#endif
    }

    for (po::variables_map::iterator iter = vm.begin(); iter != vm.end(); ++iter){
        map[iter->first.c_str()] = iter->second.value();
    }

    *ndim_def = any_cast<int>(map["ndim"]);
    switch (any_cast<int>(map["pot-type"])){
        case 1:
            *charges_def = 1;
            break;
        case 2:
            *charges_def = 2;
            break;
        default:
            *charges_def = 0;
    }
    *field_def = any_cast<int>(map["nfield"]);

    
    foreach(paramVec x, opts){
        foreach(paramPtr m, x){
            m->set(map[m->getName()]);
            map[m->getName()] = m->verify();
        }
    }

    if (vm.count("no-build")){
    	map["no-build"] = true;
    }
    if (vm.count("binary")){
        map["binary"] = true;
    }

    return true;
}

void pmap::print(std::string runParams){

    using namespace boost::gregorian;
    using namespace boost::posix_time;

	std::ofstream fp_out;
	try{
		fp_out.open(runParams.c_str());
	}
	catch(std::ofstream::failure e){
		LOG(ERROR) << "could not open the file" << std::endl;
		return;
	}

	ptime now = second_clock::local_time();
    fp_out << "#This is the set of parameters used in the previous run, performed at: "
        << to_simple_string(now)
        << std::endl;
    foreach(pairm m, map)
    {
        fp_out << m.first << " = " ;
        std::string front = "The value of " + boost::lexical_cast<std::string>(m.first) + " is: " ;
        std::string back;
        if (typeid(float) == m.second.type()){
            back =  boost::lexical_cast<std::string>(any_cast<double>(m.second));
        } else if (typeid(int) == m.second.type()){
            back = boost::lexical_cast<std::string>(any_cast<int>(m.second));
        } else if (typeid(std::string) == m.second.type()) {
            back = boost::lexical_cast<std::string>(any_cast<std::string>(m.second));
        } else if (typeid(std::vector<int>) == m.second.type()){
            foreach(int i, any_cast<std::vector<int> >(m.second)){
                back = back + boost::lexical_cast<std::string>(i) + ",";
            }
        } else if (typeid(std::vector<double>) == m.second.type()){
            foreach(double i, any_cast<std::vector<double> >(m.second)){
                 back = back + boost::lexical_cast<std::string>(i) + ",";
            }
        } else {
            back = "n/a";
        }   
        fp_out << m.first << " = " << back << "\n";
    }
    fp_out << std::endl;
    fp_out.close();
}

void pmap::print(){

    using namespace boost::gregorian;
    using namespace boost::posix_time;

	ptime now = second_clock::local_time();
    LOG(INFO) << "This is the set of parameters being used in the current run, performed at: " + to_simple_string(now);
    foreach(pairm m, map)
    {
        std::string front = "The value of " + boost::lexical_cast<std::string>(m.first) + " is: " ;
        std::string back;
        if (typeid(float) == m.second.type()){
            back =  boost::lexical_cast<std::string>(any_cast<double>(m.second));
        } else if (typeid(int) == m.second.type()){
            back = boost::lexical_cast<std::string>(any_cast<int>(m.second));
        } else if (typeid(std::string) == m.second.type()) {
            back = boost::lexical_cast<std::string>(any_cast<std::string>(m.second));
        } else if (typeid(std::vector<int>) == m.second.type()){
            foreach(int i, any_cast<std::vector<int> >(m.second)){
                back = back + boost::lexical_cast<std::string>(i) + " ";
            }
        } else if (typeid(std::vector<double>) == m.second.type()){
            foreach(double i, any_cast<std::vector<double> >(m.second)){
                 back = back + boost::lexical_cast<std::string>(i) + " ";
            }
        } else {
            back = "n/a";
        }
        LOG(INFO) << front + back;
    }
}

/*
void pmap::serialize(){
    std::ofstream ofs(any_cast<std::string>((*this).map["data_file"]).c_str());
    boost::archive::binary_oarchive oa(ofs);
    oa << (*this).map;
}

void pmap::restore(){
    boost::archive::binary_iarchive ia(std::cin);
    ia >> (*this).map;
}
*/

//TODO: this function is TROUBLE!
void pmap::map_out(anyMap &mapout){
    mapout = (*this).map;
}
}
