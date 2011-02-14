#include <boost/python.hpp>
#include "pywrapper.hpp"
namespace classical{

using namespace boost::python;

BOOST_PYTHON_MODULE(classical)
{
    def("pyrun_main",pyrun_main);
}
}
