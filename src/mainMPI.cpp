 // $Id$
/**
 * @file main.cpp
 * This is the control routine for the rate calculation. it can calculate
 * several types of rates depending on input parmeters 
 *
 * @brief controling program
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$
#include <Python.h>
#include <iostream>
#include <fstream>
#include <string>

#include "customGlog.hpp"

#include "pywrapper.hpp"
#include "config.hpp"
#include "timer.hpp"

int main(int argc, char *argv[])
{

    namespace mpi = boost::mpi;
    mpi::environment env(argc, argv);
	using namespace classical;
	using namespace timer;
    mpi::communicator world;
	if (world.rank() == 0){
	    Timer::create();
	    TimerStart("main");
	}
    std::vector<std::string> av;
    for(int i=0;i<argc;i++){
        av.push_back(*(argv+i));
    }

    simulation s(argc,av,"input1.cfg");
    int retVal = s.param_init();
    if (retVal!=0){
        return retVal;
    }
    LOG(INFO) << "building initial conditions";
    s.ic_gen();
    if (world.rank()==0)
        TimerStart("trajs");
    s.run_trajs();
    if (world.rank()==0){
        TimerStop("trajs");
        TimerStart("saving");
        s.save("text","out1.log");
        TimerStop("saving");
        TimerStart("plotting");
        boost::shared_ptr<std::vector<double> > x,y,z;
        x = s.int1D(1);
        y = s.int1Dinit(1);
        z = s.range(1);
        std::cout << z->size() << std::endl;
        for(std::size_t i=0;i<z->size();++i){
            std::cout << x->operator[](i) << " " << y->operator[](i) << " " << z->operator[](i) << std::endl;
        }
        TimerStop("plotting");
        TimerStop("main");
        TimerReport();
    }
}

