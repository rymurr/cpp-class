#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Params
#include <boost/test/unit_test.hpp>
#include <boost/any.hpp>
#include <string>
#include <map>
#include <iostream>
#include "params.hpp"

using boost::any_cast;


BOOST_AUTO_TEST_SUITE(params)

BOOST_AUTO_TEST_CASE(initparams)
{

    pmap params;
    params.map["test"]=1;
    int j=1;
    BOOST_REQUIRE_EQUAL(any_cast<int>(params.map["test"]),j);


}

BOOST_AUTO_TEST_CASE(retparams)
{
	int a=2;

	std::string str="test_config.cfg";
	char *writable = "hello --printwf 1";

    pmap params;
    params.read_params(str,a,&writable);

    int j=1;
    BOOST_REQUIRE_EQUAL(any_cast<int>(params.map["printwf"]),j);
    BOOST_REQUIRE_EQUAL(any_cast<int>(params.map["nthreads"]),j);

}

BOOST_AUTO_TEST_SUITE_END()

