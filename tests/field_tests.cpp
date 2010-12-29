#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fields
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
    field efield(test,0);
    BOOST_CHECK_CLOSE(efield(0.),1.,1E-6); 


}

BOOST_AUTO_TEST_CASE(initpotparams)
{

    anyMap test;
    std::vector<double> y,x;
    x.push_back(1.);
    test["charges"] = x;
    test["smoothing"] = 0.;
    test["rnuc"] = 0.;
    test["theta-nuc"] = 0.;
    test["phi-nuc"] = 0.;
    test["pot-type"] = 1;
    potential efield(test);
    BOOST_CHECK_CLOSE(efield(2.,0.,0.),-0.5,1E-6); 


}

BOOST_AUTO_TEST_SUITE_END()

