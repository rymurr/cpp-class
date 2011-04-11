#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fields
#include <Python.h>
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "fields.hpp"
#include "potential.hpp"
#include "input_param.hpp"
#include "coords.hpp"
namespace classical{

using boost::any_cast;


BOOST_AUTO_TEST_SUITE(fields)

BOOST_AUTO_TEST_CASE(initparams)
{

    anyMap test;
    std::vector<double> y,x;
    x.push_back(1.);
    y.push_back(0.);
    test["omega"] = x;
    test["ce"] = y;
    test["ef"] = x;
    test["fwhm"] = x;
    test["env"] = 1;
    test["tfinal"] = 100.;
    test["tinitial"] = 1.;
    boost::shared_ptr<Field> efield(fieldFactory(test));
    BOOST_CHECK_CLOSE(efield->operator()(0.),1.,1E-6);


}

BOOST_AUTO_TEST_CASE(initpotparams)
{

    anyMap test;
    std::vector<double> y(3,0),x;
    x.push_back(1.);
    test["charges"] = x;
    test["smoothing"] = 0.;
    test["rnuc"] = 0.;
    test["theta-nuc"] = 0.;
    test["phi-nuc"] = 0.;
    test["pot-type"] = 1;
    y[0] = 2.;
    Coords<double> z(y);
    boost::shared_ptr<Potential> efield(potentialFactory(test));
    BOOST_CHECK_CLOSE(efield->operator ()(z),-0.5,1E-6);


}

BOOST_AUTO_TEST_SUITE_END()
}

