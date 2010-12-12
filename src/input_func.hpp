
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>

#define foreach     BOOST_FOREACH

typedef std::map<std::string,boost::any> anyMap;

using boost::any_cast;

namespace po = boost::program_options;

void params_in(int, char**, anyMap&);
