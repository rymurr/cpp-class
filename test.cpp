
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime> 
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <boost/foreach.hpp>

#define foreach     BOOST_FOREACH
typedef boost::minstd_rand base_generator_type; 
int main(int argc, char** argv){


    base_generator_type generator(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<> uni_dist(-1,1);
    boost::variate_generator<base_generator_type&, boost::uniform_real<> > uni(generator, uni_dist);

    //double trajs = std::accumulate(trajs_.begin(),trajs_.end(),0);

    for (int i=0;i<trajs_.size();i++){
        double range = abs(ranges_[i].first-ranges_[i].second)*0.5;
        for (int j=0;j<trajs_[i]){
            initConditions_[i][j] = range*uni();
        }
    }

    for (int i=0;i<trajs_.size();i++){
        for (int j=0;j<trajs_[i]){
            std::cout << initConditions_[i][j];
        }
    }
}
}
