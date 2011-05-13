#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Binner

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>

//#include "integrator.hpp"
//#include "force.hpp"
#include "coords.hpp"
//#include "potential.hpp"
#include "icgen.hpp"
#include "timer.hpp"
#include "binner.hpp"

BOOST_AUTO_TEST_SUITE(binner)

BOOST_AUTO_TEST_CASE(nullSample){
    using namespace timer;
    using namespace classical;
    Timer::create();

    anyMap test;

    test["variance"] = 1.;
    std::vector<double> xx(4,0.);
    std::vector<int> yy(4,50);
    test["dims"] = yy;
    test["means"] = xx;
    test["nobuild"] = true;
    test["dist-type"] = 1;
    anyMap test2;
    test2["weight-func"] = 2;
    test2["ef"] = std::vector<double>(1,0.6666666666);
    test2["ip"] = 1.0;
    test2["sigmas"] = boost::shared_ptr<std::vector<double> >(new std::vector<double>(4,1.));

    icgenerator gen(test,&test2);
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    Binner bin(N,V);
    Coords vals(xx);
    TimerStart("main");
    for(int i=0;i<6250000;++i){
       gen.retIC(vals);
       double w = gen.retWeight(vals);
       //std::cout << w << std::endl;
       bin(vals,w);
    }
    boost::shared_ptr<std::vector<double> > intX(bin.int1D(1));
    for(std::size_t i=0;i<intX->size();++i){
        std::cout << intX->operator[](i) << std::endl;
    }
    TimerStop("main");
    TimerReport();
}

BOOST_AUTO_TEST_SUITE_END()
