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


//#include <boost/assign/list_inserter.hpp>
//#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

#include "input_func.hpp"


#define foreach     BOOST_FOREACH

using boost::any_cast;
using namespace boost;
        

typedef std::map<std::string,boost::any> anyMap;
//this namespace brings boost::program options into scope as po


class pmap {

    public:
        pmap();
        
        anyMap map;

        virtual ~pmap();
        
        void read_params(const std::string, int, char**);

        void print(std::string);

        void print();

        void validate_map();


    private:
        
        void generic_options(void);
        
        void time_and_space(void);
    
        void wfpot(void);
};

void parse_string(std::string, int size, std::string param, boost::shared_ptr<std::vector<int> >);


#endif
