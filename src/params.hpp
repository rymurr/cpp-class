// $Id$
/**
 * @file params.h
 * This is a self-contained function to read in a set of parameters and put them into a map
 * the map is then passed back to the calling function
 * a few more parameters are handed to and from mostly for this program only
 * it implements boost program_options for command line and config file input
 *
 * @brief params definitions
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$

#ifndef PARAMS_H_
#define PARAMS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <boost/program_options.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
///defines foreach iterating function from boost
#define foreach     BOOST_FOREACH

using boost::any_cast;

//this namespace brings boost::program options into scope as po
namespace po = boost::program_options;

class pmap: public std::map<std::string,boost::any> {


};


#endif
