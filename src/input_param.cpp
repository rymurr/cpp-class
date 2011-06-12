
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

int pmap::read_params(const std::string fname, const int argc, std::vector<std::string> &argv){

    //TODO: need locations for input files: numerical input
    //TODO: need to add an auxilliary variable to accept user variables. Right now everything gets put to standard files
    paramVec general_opts, time_opts, wf_opts, field_opts, run_opts, plot_opts;
    
    boost::shared_ptr<int> ndim_def(new int), charges_def(new int), field_def(new int);
    defMap dist_map, traj_map, inwf_map, inpot_map, field_map, weight_map, sim_map, int_map;

    boost::assign::insert(int_map)
        (1,"Null integrator, does nothing")
        (3,"Symplectic Integrator")
        (4,"Adaptive Runge-Kutta")
        ;
    boost::assign::insert(sim_map)
        (0, "None")
#ifdef OPENMP_FOUND
        (1, "OpenMP")
#endif
#ifdef MPI_FOUND
        (2, "MPI ")
#endif
        ;
    boost::assign::insert(dist_map)
        (1,"Monte-Carlo")
        (2,"linear")
        (3,"staged-linear (not implemented)")
        ;
    boost::assign::insert(weight_map)
        (1,"Unit weight")
        (2,"ADK atomic rate with gaussian weight")
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
        (5,"null potential")
        ;
    boost::assign::insert(field_map)
        (1,"constant cosine field")
        (2,"static field")
        (3,"gaussian envelope")
        (4,"sine squard envelope")
        (5,"numerical form")
        ;

    general_opts.push_back(intRunPtr(new intRun("The Number of dimensions to be used in the calculation", "ndim", 4, 100, 1)));
    general_opts.push_back(intListPtr(new intList("The number of trajectories in each dimension. comma separated list.", "dims", "1,1,1,1",ndim_def)));
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
    wf_opts.push_back(statePtr(new state_param("Type of ionization weight function", "weight-func", 2, weight_map)));
    wf_opts.push_back(doubleListPtr(new doubleList("charge on core(s). list of numbers for each core only needed for H-like and H2-like potential. 1 is for singly ionized etc.", "charges", "1",charges_def)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("Smoothing parameter to avoid coulomb singularity. Should be small and is only needed when using analytic potentials.", "smoothing", 1E-4, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("Internuclear separation in atomic units, only needed for analytic potentials", "rnuc", 0.0, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("x-y plane orientation of molecule", "phi-nuc", 0.0, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("alignment of molecule wrt the z-axis.", "theta-nuc", 0.0, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("ionization potential of target, given in atomic units", "ip", 0.5, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("state of atomic or molecular system (not implemented)", "state", 0.01, 10E10, -10E10)));
    wf_opts.push_back(doubleRunPtr(new doubleRun("escape range: the distance from the origin which an electron can consider being ionized", "escape-range", 1E-5, 10E10, 0)));
    
    field_opts.push_back(intRunPtr(new intRun("Number of fields, every option below is given as a comma separated list of options for each field. Currently only 1 field is allowed", "nfield", 1, 100, 1)));
    field_opts.push_back(statePtr(new state_param("Type of fields", "env", 1,field_map)));
    field_opts.push_back(doubleListPtr(new doubleList("Field strengths given in atomic units", "ef", "0.01",field_def)));
    field_opts.push_back(doubleListPtr(new doubleList("frequencies given in atomic units", "omega", "0.057",field_def)));
    field_opts.push_back(doubleListPtr(new doubleList("FWHM of envelope, ignored for static or constant field.", "fwhm", "0",field_def)));
    field_opts.push_back(doubleListPtr(new doubleList("initial CE phase of each field given in degrees", "ce", "0",field_def)));
    field_opts.push_back(intListPtr(new intList("polarization direction, 1 = x, 2 = y, 3 = z,", "pol", "3",field_def)));

    run_opts.push_back(intRunPtr(new intRun("Number of concurrent trajectories using OpenMP", "nthreads", 1, 100, 1)));
    run_opts.push_back(intRunPtr(new intRun("Number of nodes using MPI", "mklthreads", 1, 100, 1)));
    run_opts.push_back(doubleRunPtr(new doubleRun("Absolute error", "abserr", 1.E-9, 10E10, -10E10)));
    run_opts.push_back(doubleRunPtr(new doubleRun("Relative error", "relerr", 1E-7, 10E10, -10E10)));
    run_opts.push_back(statePtr(new state_param("Type of parallel run", "parallel", 0, sim_map)));
    run_opts.push_back(statePtr(new state_param("Type of integrator to use", "int-type", 4, int_map)));

    plot_opts.push_back(intRunPtr(new intRun("Number of bins in final distrbution", "plot-nums", 256, 10E6, 1)));
    plot_opts.push_back(doubleRunPtr(new doubleRun("absolute range of saved distribution in atomic units of momentum", "plot-ranges", 6., 10E10, 0)));

    std::vector<boost::shared_ptr<po::options_description> > opts_desc;
    opts_desc.push_back(boost::shared_ptr<po::options_description>(new po::options_description("Generic Options")));
    opts_desc.push_back(boost::shared_ptr<po::options_description>(new po::options_description("Time Grid")));
    opts_desc.push_back(boost::shared_ptr<po::options_description>(new po::options_description("Wave function and potential")));
    opts_desc.push_back(boost::shared_ptr<po::options_description>(new po::options_description("Field options")));
    opts_desc.push_back(boost::shared_ptr<po::options_description>(new po::options_description("Plotting specific options")));
    opts_desc.push_back(boost::shared_ptr<po::options_description>(new po::options_description("Run specific options")));

    opts_desc[0]->add_options()
	        ("help", "displays help message");
    opts_desc[0]->add_options()
	        ("help-glog", "displays help messages for google glog, the logging interface");
    opts_desc[5]->add_options()
    		("no-build", "builds the initial conditions on the fly, without holding entire set in memory");
    opts_desc[5]->add_options()
            ("no-bin", "do not create distribution while running trajectories, bining of distribution is done in separate step, not valid when doing parallel calculations");
    opts_desc[5]->add_options()
            ("no-store", "do not store trajectories separately just add them to the distribution(mutually exclusive with no-bin, this takes precedence)");
    opts_desc[5]->add_options()
            ("binary", "serialize in binary(OS-dependent), if not set serializes to text(default)");
    opts_desc[5]->add_options()
            ("watch", "turns on observer for RK integrator...lots of output!");
    std::vector<paramVec> opts;
    opts.push_back(general_opts);
    opts.push_back(time_opts);
    opts.push_back(wf_opts);
    opts.push_back(field_opts);
    opts.push_back(plot_opts);
    opts.push_back(run_opts);
    for(std::size_t i=0;i<opts.size();++i){
        foreach(paramPtr m, opts[i]){
            m->genParam(*(opts_desc[i]));
        }
    }
    po::options_description cmdline, config, visible("Allowed options");
    foreach(boost::shared_ptr<po::options_description> x, opts_desc){
        visible.add(*x);
    }
    po::variables_map vm;
    char** av=new char*[argc];
    for(int i=0;i<argc;i++){
        av[i] = &argv[i][0];
    }
    po::parsed_options parsed = po::command_line_parser(argc, av).options(visible).allow_unregistered().run();
    delete[] av;
    po::store(parsed, vm);
    po::notify(vm);

    std::ifstream ifs(fname.c_str());
    po::store(parse_config_file(ifs, visible), vm);
    po::notify(vm);

    std::vector<std::string> to_gflags = po::collect_unrecognized(parsed.options,po::include_positional);
    std::vector<std::string>::iterator it;
    it = to_gflags.begin();
    to_gflags.insert(it,argv[0]);
    //to_gflags.push_back("--logtostderr");

//    delete[] av2;
    std::string usage = "Options included by google-glog for logging purposes";
#ifdef GFlags_FOUND
    google::SetUsageMessage(usage);
#endif
    if (vm.count("help")){
        std::cout << visible << "\n";
        return 10;
    }
    if (vm.count("help-glog")){
        to_gflags.push_back("--help");
        int gsize = to_gflags.size();
        char** av2=new char*[gsize];
        for(int i=0;i<gsize;i++){
            av2[i] = &to_gflags[i][0];
        }
#ifdef GFlags_FOUND
        google::ParseCommandLineFlags(&gsize, &av2, true);
#endif
        return 10;
    } else {
        int gsize = to_gflags.size();
        char** av2=new char*[gsize];
        for(int i=0;i<gsize;i++){
            av2[i] = &to_gflags[i][0];
        }
#ifdef GFlags_FOUND
        google::ParseCommandLineFlags(&gsize, &av2, true);
#endif
    }


    for (po::variables_map::iterator iter = vm.begin(); iter != vm.end(); ++iter){
        map[iter->first.c_str()] = iter->second.value();
        /*std::cout << iter->first << " = " ;
            std::string front = "The value of " + boost::lexical_cast<std::string>(iter->first) + " is: " ;
            std::string back;
            if (typeid(float) == iter->second.value().type()){
                back =  boost::lexical_cast<std::string>(any_cast<double>(iter->second.value()));
            } else if (typeid(int) == iter->second.value().type()){
                back = boost::lexical_cast<std::string>(any_cast<int>(iter->second.value()));
            } else if (typeid(std::string) == iter->second.value().type()) {
                back = boost::lexical_cast<std::string>(any_cast<std::string>(iter->second.value()));
            } else if (typeid(std::vector<int>) == iter->second.value().type()){
                foreach(int i, any_cast<std::vector<int> >(iter->second.value())){
                    back = back + boost::lexical_cast<std::string>(i) + ",";
                }
            } else if (typeid(std::vector<double>) == iter->second.value().type()){
                foreach(double i, any_cast<std::vector<double> >(iter->second.value())){
                     back = back + boost::lexical_cast<std::string>(i) + ",";
                }
            } else {
                back = "n/a";
            }
            std::cout << iter->first << " = " << back << "\n"; std::cout << std::endl;*/
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

    if (vm.count("no-bin")){
        map["no-bin"] = true;
    }

    if (vm.count("no-store")){
        if (boost::any_cast<int>(map["parallel"]) == 2){
            map["sim-type"] = 5;
        } else if (boost::any_cast<int>(map["parallel"]) == 1) {
            map["sim-type"] = 4;
        } else {
            map["sim-type"] = 2;
        }

    } else {
        if(vm.count("no-bin")){
            map["sim-type"] = 3;
        } else {
            map["sim-type"] = 1;
        }
    }

    if(vm.count("watch")){
        map["watch"] = true;
    }
    return 0;
}

void pmap::print(const std::string runParams) const {

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
        if (typeid(double) == m.second.type()){
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
        fp_out << back << "\n";
    }
    fp_out << std::endl;
    fp_out.close();
}

void pmap::print() const {

    using namespace boost::gregorian;
    using namespace boost::posix_time;

	ptime now = second_clock::local_time();
    LOG(INFO) << "This is the set of parameters being used in the current run, performed at: " + to_simple_string(now);
    foreach(pairm m, map)
    {
        std::string front = "The value of " + boost::lexical_cast<std::string>(m.first) + " is: " ;
        std::string back;
        if (typeid(double) == m.second.type()){
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
boost::shared_ptr<anyMap> pmap::map_out(){
    return boost::shared_ptr<anyMap>(new anyMap((*this).map));
}
}
