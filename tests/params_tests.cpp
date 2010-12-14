#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Params
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
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
	int a=3;

	std::string str="test_config.cfg";
	char *writable = "hello -n 1";

    pmap params;
    params.read_params(str,a,&writable);

    int j=1, i=2;
    BOOST_REQUIRE_EQUAL(any_cast<int>(params.map["ndim"]),i);
    BOOST_REQUIRE_EQUAL(any_cast<int>(params.map["nthreads"]),j);
    params.print();

}

BOOST_AUTO_TEST_CASE(strSplit)
{

    boost::shared_ptr<std::vector<int> > dims(new std::vector<int>);
    parse_string("1,1", 2, "dims",  dims); 

    BOOST_REQUIRE_EQUAL((*dims)[0],1);
    BOOST_REQUIRE_EQUAL((*dims)[1],1);


}

BOOST_AUTO_TEST_SUITE_END()

