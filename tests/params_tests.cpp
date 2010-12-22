#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Params
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <string>
#include <map>
#include <iostream>
#include "input_param.hpp"

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
	char *writable = "hello --ndim 1";

    pmap params;
    params.read_params(str,a,&writable);

    int j=1, i=2;
    BOOST_REQUIRE_EQUAL(any_cast<int>(params.map["ndim"]),i);
    params.print();

}

BOOST_AUTO_TEST_CASE(paramstpl)
{
    boost::shared_ptr<int> x(new int(2));

    std::map<int,std::string> testmap;
    testmap[1] = "poo";
    testmap[2] = "crap";
    param<int> test("this is a test", "testing", 1);
    run_param<double> test2("this", "is", 1., 9., 0.);
    state_param test3("test", "is", 1, testmap);
    list_param<int> test4("list desc", "list", "1,1", x);
    test4.verify();
    test3.verify();
    test2.verify();
    test3.set(82);
    test2.set(50.);
    *x = 4;
    test4.set("1,1,1");
    test.print();
    test2.print();
    test3.print();
    test4.print();

    std::map<std::string,boost::any> tstmap;
    tstmap["a"] = test2;
    any_cast<run_param<double> >(tstmap["a"]).set(22.2);
    any_cast<run_param<double> >(tstmap["a"]).print();


    boost::any testany;
    tstmap["b"] =boost::shared_ptr<run_param<double> >(new run_param<double>(test2));
    any_cast<boost::shared_ptr<run_param<double> > >(tstmap["b"])->set(23.332);
    any_cast<boost::shared_ptr<run_param<double> > >(tstmap["b"])->print();
    BOOST_REQUIRE_EQUAL(*(any_cast<boost::shared_ptr<run_param<double> > >(tstmap["b"])->actualValue),23.332);
}

BOOST_AUTO_TEST_SUITE_END()

