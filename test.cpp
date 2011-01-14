
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime> 
#include <algorithm>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

#define foreach     BOOST_FOREACH
typedef boost::minstd_rand base_generator_type; 
int main(int argc, char** argv){
    using namespace boost::lambda;
    std::vector<int> x;
    x.push_back(1);
    x.push_back(2);
    x.push_back(3);
    x.push_back(4);
    x.push_back(5);
    std::vector<int> y = x;
    int tot=1;
    std::for_each(x.begin(),x.end(),tot*=_1);
    std::cout <<tot << "\n";
    std::transform(x.begin(),x.end(),y.begin(),_1*=_2);
    for (int i=0;i<5;i++){
        std::cout << x[i] << " " << y[i] << "\n";
    }
}
