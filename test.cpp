



#include "src/params2.hpp"
#include <iostream>
#include <string>
#include <map>

int main(int argv, char** argc){

    std::map<int,std::string> testmap;
    testmap[1] = "poo";
    testmap[2] = "crap";
    param<int> test("this is a test", "testing", "testval", 1);
    run_param test2("this", "test", "is", 1., 2., 10., 5.);
    state_param test3("this", "test", "is", 1, 4, testmap);
    test2.verify();
    test3.verify();
    test.print();
    test2.print();
    test3.print();


}
