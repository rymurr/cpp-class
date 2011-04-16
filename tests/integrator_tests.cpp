#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Integrate

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <map>
#include <string>
#include <iostream>

#include "integrator.hpp"
#include "force.hpp"
#include "coords.hpp"
#include "potential.hpp"

BOOST_AUTO_TEST_SUITE(integrate)

BOOST_AUTO_TEST_CASE(testVoid){

    using namespace classical;
    anyMap test;
    test["int-type"] = 1;
    Integrator x(test);
    Cpair y(Coords(3,0),0.);

    Cpair z(x(y));
    BOOST_CHECK_CLOSE(z.first[0], y.first[0], 10E-5);
    BOOST_CHECK_CLOSE(z.first[1], y.first[1], 10E-5);
    BOOST_CHECK_CLOSE(z.first[2], y.first[2], 10E-5);

}

BOOST_AUTO_TEST_CASE(testSymFieldFree){
    using namespace classical;
    using boost::any_cast;
    anyMap test;
    test["int-type"] = 2;
    test["tinitial"] = 0.;
    test["dt"] = 0.1;
    test["potPtr"] = forceFactory(5);
    test["kinPtr"] = forceFactory(6);
    Integrator x(test);
    Coords y(4,0);
    y[3] = 1.;
    Cpair z=make_pair(y,20.);
    z = x(z);
    BOOST_CHECK_CLOSE(z.first[1],20.,10E-5);
}

BOOST_AUTO_TEST_CASE(testSymHatom){
    using namespace classical;
    using boost::any_cast;
    anyMap test;
    test["int-type"] = 2;
    test["tinitial"] = 0.;
    test["dt"] = 0.1;
    std::vector<double> xx(1,1.);
    test["charges"] = xx;
    test["smoothing"] = 1E-8;
    test["rnuc"] = 0.;
    test["theta-nuc"] = 0.;
    test["phi-nuc"] = 0.;
    test["pot-type"] = 1;
    test["ip"] = 0.5;
    test["potPtr"] = forceFactory(test);
    test["kinPtr"] = forceFactory(6);
    boost::shared_ptr<Potential> potPot = potentialFactory(test);
    boost::shared_ptr<Potential> kinPot = potentialFactory();
    Integrator x(test);
    Coords y(4,0);
    y[0] = 1.;
    y[3] = 1.;
    double t=0, tfin = 20.;
    Cpair z=make_pair(y,t);
    Coords xxx(2), pp(2);
    z.second = tfin;
    z = x(z);
    xxx[0] = z.first[0]; xxx[1] = z.first[1];pp[0] = z.first[2]; pp[1]=z.first[3];
    double energy = potPot->operator()(xxx) + kinPot->operator()(pp);
    BOOST_CHECK_CLOSE(energy,-0.5,10E-5);
}
BOOST_AUTO_TEST_SUITE_END()
