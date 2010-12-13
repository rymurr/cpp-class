
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

#include <boost/program_options.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/foreach.hpp>
#include <boost/any.hpp>

#define foreach     BOOST_FOREACH

typedef std::map<std::string,boost::any> anyMap;
typedef std::pair<std::string,boost::any> pairm;

using boost::any_cast;
using namespace boost::gregorian;
using namespace boost::posix_time;


namespace po = boost::program_options;

void params_in(int, char**, anyMap&);

void print_cfg(std::string, const anyMap);

void print_cli(const anyMap);
