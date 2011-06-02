#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Sim

#include <Python.h>
#include <string>
#include <boost/test/unit_test.hpp>
#include <pywrapper.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(simulate)

BOOST_AUTO_TEST_CASE(init){
    int argc=2;

    using namespace classical;
    std::string fname_in = "test_in.cfg";
    std::vector<std::string> av;
    av.push_back("cpp-class");
    av.push_back("--logtostderr");

    simulation sim(argc,av,fname_in);
    sim.param_init();
    sim.ic_gen();
    sim.run_trajs();
    boost::shared_ptr<std::vector<double> > x = sim.int1D(1);
    boost::shared_ptr<std::vector<double> > y = sim.int1Dinit(1);
    boost::shared_ptr<std::vector<double> > z = sim.range(1);
    for(std::size_t i=0;i<y->size();++i){
        std::cout << y->operator[](i) << " " << x->operator[](i) << " " << z->operator[](i) << std::endl;
        BOOST_CHECK_CLOSE(y->operator[](i), x->operator[](i),1E-5);
    }
}

BOOST_AUTO_TEST_SUITE_END()
