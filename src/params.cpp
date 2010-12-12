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


pmap::pmap(){

}

pmap::~pmap(){

}

void pmap::read_params(const std::string fname, int ac, char** av){

	(*this).map["ConfigFile"] = fname;
	//anyMap retMap;
    std::cout << "hello" <<std::endl;
	params_in(ac, av, (*this).map);
    std::cout << "hello" <<std::endl;
	//(*this).map = retMap;

}


