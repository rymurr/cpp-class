
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <boost/foreach.hpp>

#define foreach     BOOST_FOREACH

int main(int argc, char** argv){
        std::vector<std::string> test;
        for(int i=0;i<argc;i++){
            test.push_back(*(argv+i));
        }
        char** out=new char*[argc]; 
        for(int i=0;i<argc;i++){
            out[i] = &test[i][0];
        }

        std::cout << *argv << *(argv+1);
        std::cout << test[0] << test[1];
        std::cout << *out << *(out+1);


}
