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

#include "params.hpp"


int main(int argc, char **argv)
{
    pmap params;
    params.read_params("run_params.cfg", argc, argv);
    params.validate_map();
    params.print();
}

