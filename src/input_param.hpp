// $Id$
/**
 * @file input_param.hpp
 * This class reads in a set of parameters and stores them in a std::map
 * The map can be exported for use in the main numerical algorithm
 * boost program_options is used along with custom verify functions
 *
 * @brief params definitions
 *
 * @author Ryan Murray
 * @version 2.00
 */
// $Log$

#ifndef IPARAMS_H_
#define IPARAMS_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <typeinfo>

#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/assign/list_inserter.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/serialization/map.hpp>
#ifdef Gflags
    #include <gflags/gflags.h>
#endif
#include "customGlog.hpp"

#include "params.hpp"
namespace classical{

#define foreach     BOOST_FOREACH

using boost::any_cast;
namespace po = boost::program_options;

typedef std::map<std::string,boost::any> anyMap;
typedef std::vector<boost::shared_ptr<param> > paramVec;
typedef std::map<int,std::string> defMap;
typedef std::vector<boost::any> anyVec;
typedef std::pair<std::string,boost::shared_ptr<param> > pairGen;
typedef std::pair<std::string,boost::any > pairm;

typedef run_param<int> intRun;
typedef run_param<double> doubleRun;
typedef list_param<int> intList;
typedef list_param<double> doubleList;

typedef boost::shared_ptr<param> paramPtr;
typedef boost::shared_ptr<intRun > intRunPtr;
typedef boost::shared_ptr<doubleRun > doubleRunPtr;
typedef boost::shared_ptr<intList > intListPtr;
typedef boost::shared_ptr<doubleList > doubleListPtr;
typedef boost::shared_ptr<state_param> statePtr;
typedef boost::shared_ptr<file_param> filePtr;

class pmap {

    private:
        anyMap map;

    public:
        pmap();
        
        ~pmap();

        void print();
        
        void print(std::string);
        
        void map_out(anyMap&);

//        void serialize();
    
//        void restore();

        bool read_params(std::string, int, std::vector<std::string>&);
};

}


#endif
