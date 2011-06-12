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

	using namespace classical;
	using namespace timer;
    Timer::create();
    TimerStart("main");
    std::vector<std::string> av;
    for(int i=0;i<argc;i++){
        av.push_back(*(argv+i));
    }
    simulation s(argc,av,"input.cfg");
    int retVal = s.param_init();
    if (retVal!=0){
        return retVal;
    }
    LOG(INFO) << "building initial conditions";
    s.ic_gen();
    TimerStart("trajs");
    s.run_trajs();
    TimerStop("trajs");
    TimerStart("saving");
    s.save("text","out.log");
    TimerStop("saving");
    TimerStart("plotting");
    boost::shared_ptr<std::vector<double> > x,y,z;
    boost::shared_ptr<boost::multi_array<double,2> > xx,yy;
    xx = s.int2D(1,2);
    yy = s.int2DInit(1,2);
    z = s.range(1);
    /*
    std::cout << z->size() << std::endl;
    for(std::size_t i=0;i<z->size();++i){
        std::cout << z->operator[](i) << std::endl;
    } 
    for(std::size_t i=0;i<xx->shape()[0];++i){
        for(std::size_t j=0;j<xx->shape()[1];++j){
            std::cout << (*xx)[i][j] << " " << (*yy)[i][j] << std::endl;
        }
    }*/
    TimerStop("plotting");
    TimerStop("main");
    TimerReport();
}

