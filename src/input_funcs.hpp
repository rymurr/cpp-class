#ifndef IFUNCS_H_
#define IFUNCS_H_

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
#include <boost/assign/list_inserter.hpp>

//#include "input_func.hpp"
#include "params.hpp"
namespace classical{

#define foreach     BOOST_FOREACH

using boost::any_cast;
using namespace boost;
namespace po = boost::program_options;

 
typedef std::map<std::string,boost::any> anyMap;
typedef std::map<int,std::string> defMap;
typedef std::vector<boost::any> anyVec;
typedef std::pair<std::string,boost::any> pairm;

typedef boost::shared_ptr<run_param<int> > intRunPtr;
typedef boost::shared_ptr<run_param<double> > doubleRunPtr;
typedef boost::shared_ptr<list_param<int> > intListPtr;
typedef boost::shared_ptr<list_param<double> > doubleListPtr;
typedef boost::shared_ptr<state_param> statePtr;

typedef run_param<int> intRun;
typedef run_param<double> doubleRun;
typedef list_param<int> intList;
typedef list_param<double> doubleList;


pairm validate (pairm&, boost::any);

void gen_param(pairm, po::options_description&);

}

#endif
