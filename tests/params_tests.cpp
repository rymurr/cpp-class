#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Params
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include <string>
#include <map>
#include <iostream>
#include "params.hpp"

using boost::any_cast;

//typedef std::map<std::string,boost::any> pmap;

BOOST_AUTO_TEST_SUITE(params)

BOOST_AUTO_TEST_CASE(initparams)
{

    pmap params();
    params["test"]=1;
    int j=1;
    BOOST_REQUIRE_EQUAL(any_cast<int>(params["test"]),j);

}

BOOST_AUTO_TEST_SUITE_END()

