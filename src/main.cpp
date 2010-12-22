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

#include "pywrapper.hpp"


int main(int argc, char **argv)
{
    std::string av = *argv;
    if (!run_main(argc, av, "run_params.cfg")){return 10;};
}

