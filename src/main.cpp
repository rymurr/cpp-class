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
#include <iostream>
#include <fstream>
#include <string>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "pywrapper.hpp"

int main(int argc, char *argv[])
{

	using namespace classical;
    std::vector<std::string> av;
    for(int i=0;i<argc;i++){
        av.push_back(*(argv+i));
    }
    
    if (!run_main(argc, av, "run_params.cfg")){return 10;};
}

