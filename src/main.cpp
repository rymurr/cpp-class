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
#include "fields.hpp"

int main(int argc, char *argv[])
{
    //if (!run_main(argc, argv, "run_params.cfg")){return 10;};
    anyMap map;
    std::vector<double> y,x;
    x.push_back(1.);
    y.push_back(0.);
    map["omega"] = x;
    map["ce"] = y;
    map["ef"] = x;
    map["fwhm"] = x;
    map["env"] = 1;
    field efield(map,0);
    for (int i = 0; i < 1000; i++) {
        std::cout << i*0.1 << " " << efield(i*0.1) << std::endl;
    }
}

