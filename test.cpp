



#include "src/params2.hpp"
#include <iostream>
#include <string>
#include <map>
#include <boost/any.hpp>

using boost::any_cast;

int main(int argv, char** argc){

    std::map<int,std::string> testmap;
    testmap[1] = "poo";
    testmap[2] = "crap";
    param<int> test("this is a test", "testing", "testval", 1);
    run_param<double> test2("this", "test", "is", 1., 2., 10., 0.);
    state_param test3("this", "test", "is", 1, 2, testmap);
    list_param<int> test4("fun" , "times", "today", "1,1", "1,1", 2);
    test2.verify();
    test3.verify();
    test4.verify();
    test.print();
    test2.print();
    test3.print();
    test4.print();

    boost::any test5;
    test5 = test4;
    (any_cast<list_param<int> >(test5)).print();
    std::cout << test5.type().name() << test4.name ;


}
