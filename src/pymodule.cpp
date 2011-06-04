

#include <boost/python.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/list.hpp>

#include "pywrapper.hpp"
#include "config.hpp"

#ifdef PYTHONLIBS_FOUND
namespace classical{

using namespace boost::python;

//int pyrun_main(int argc,boost::python::list argv, std::string fname);



BOOST_PYTHON_MODULE(classical)
{
    //def("pyrun_main",pyrun_main);
    class_<simulation>("simulation",init<int,boost::python::list,std::string>())
        .def("param_init",&simulation::param_init)
        .def("ic_gen",&simulation::ic_gen)
        .def("run_trajs",&simulation::run_trajs)
        .def("run_bins",&simulation::run_bins)
        .def("bin1D",&simulation::int1DPython)
        .def("bin1DInit",&simulation::int1DPythonInit)
        .def("bin2D",&simulation::int2DPython)
        .def("bin2DInit",&simulation::int2DPythonInit)
        .def("bin3D",&simulation::int3DPython)
        .def("bin3DInit",&simulation::int3DPythonInit)
        .def("range",&simulation::rangePython)
        .def("rangeInit",&simulation::rangeInitPython)
    ;
}
}

#endif
