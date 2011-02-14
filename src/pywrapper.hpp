#include <string>
#include <vector>

#include <boost/python/extract.hpp>
#include <boost/python/list.hpp>
#include "input_param.hpp"
#include "fields.hpp"

namespace classical{

int run_main(int argc, std::vector<std::string> argv, std::string fname);
int pyrun_main(int argc,boost::python::list argv, std::string fname);
}
