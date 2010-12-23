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
#include "input_param.hpp"

using boost::any_cast;


BOOST_AUTO_TEST_SUITE(fields)

BOOST_AUTO_TEST_CASE(initparams)
{

    pmap test;
    std::vector<double> y,x;
    x.push_back(1.);
    y.push_back(0.);
    test.map["omega"] = x;
    test.map["ce"] = y;
    test.map["ef"] = x;
    test.map["fwhm"] = x;
    test.map["env"] = 1;
    field efield(test,0);
    BOOST_CHECK_CLOSE(efield(0.),1.,1E-6); 


}

BOOST_AUTO_TEST_SUITE_END()

