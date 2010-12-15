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

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <typeinfo>


//#include <boost/assign/list_inserter.hpp>
//#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>

//#include "input_func.hpp"
#include "params2.hpp"

#define foreach     BOOST_FOREACH

using boost::any_cast;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
namespace po = boost::program_options;

 
typedef std::map<std::string,boost::any> anyMap;
typedef std::vector<boost::any> anyVec;
typedef std::pair<std::string,boost::any> pairm;


pairm validate (pairm);

void gen_param(pairm, po::options_description&);

class pmap {

    private:
        anyMap general_opts;

    public:
        anyMap map;

        pmap();
        
        virtual ~pmap();

        void print();
        
        void validate_map();

        void print(std::string);

        void read_params(std::string, int, char**);
};


#endif
