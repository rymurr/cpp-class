



#include "src/params2.hpp"
#include <iostream>
#include <string>
#include <map>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

using boost::any_cast;

int main(int argv, char** argc){

    /*
    boost::shared_ptr<int> x(new int(2));
   
    std::map<int,std::string> testmap;
    testmap[1] = "poo";
    testmap[2] = "crap";
    param<int> test("this is a test", "testing", 1);
    */
    run_param<double> test2("this", "is", 1., 9., 0.);
    /*
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

    tstmap["a"] = test2;
//    tstmap["b"] = &test2;
    any_cast<run_param<double> >(tstmap["a"]).set(22.2);
    any_cast<run_param<double> >(tstmap["a"]).print();
//    any_cast<list_param<int> >(tstmap["a"]).print();
*/

    std::map<std::string,boost::any> tstmap;
    boost::any testany;
    tstmap["b"] =boost::shared_ptr<run_param<double> >(new run_param<double>(test2));
    any_cast<boost::shared_ptr<run_param<double> > >(tstmap["b"])->set(23.332);
    any_cast<boost::shared_ptr<run_param<double> > >(tstmap["b"])->print();
  
/*
    test2.set(50.);
    boost::shared_ptr<boost::any> testptr;
    testptr = boost::shared_ptr<boost::any>(new boost::any(test2));
    *testptr = test2;
    (any_cast<run_param<double> >(*testptr))->set(23.3);
    (any_cast<run_param<double> >(*testptr))->print();
*/

}
