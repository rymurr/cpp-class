#ifndef PYTHON
#include <boost/python.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/list.hpp>

#include "pywrapper.hpp"
namespace classical{

using namespace boost::python;

int pyrun_main(int argc,boost::python::list argv, std::string fname);

BOOST_PYTHON_MODULE(classical)
{
    def("pyrun_main",pyrun_main);
}

int pyrun_main(int argc, boost::python::list argv, std::string fname){

    std::size_t n = boost::python::len(argv);
    std::vector<std::string> av;
    for (unsigned int i = 0; i < n; i++) {
        av.push_back(boost::python::extract<std::string>(argv[i]));
    }

    if (!run_main(argc, av, fname)){return 0;}
    return 1;
}
}
}
#endif
