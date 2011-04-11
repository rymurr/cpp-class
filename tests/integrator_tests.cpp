#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Integrate

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <map>
#include <string>
#include <iostream>

#include "integrator.hpp"
#include "coords.hpp"

BOOST_AUTO_TEST_SUITE(integrate)

BOOST_AUTO_TEST_CASE(testVoid){

    using namespace classical;
    anyMap test;
    test["int-type"] = 1;
    Integrator x(test);
    Coords<double> y(3,0);

    Coords<double> z(x(y,0.));
    BOOST_CHECK_CLOSE(z[0], y[0], 10E-5);
    BOOST_CHECK_CLOSE(z[1], y[1], 10E-5);
    BOOST_CHECK_CLOSE(z[2], y[2], 10E-5);

}

BOOST_AUTO_TEST_SUITE_END()
