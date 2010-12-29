
#include "icgen.hpp"

using boost::any_cast;     

icgenerator::icgenerator(){}

icgenerator::~icgenerator(){}

icgenerator::icgenerator(anyMap params){

    ranges_ = any_cast<std::vector<std::pair<double,double> > >(params["ranges"]);
    trajs_ = any_cast<std::vector<double> >(params["dims"]);
    tType_ = any_cast<int>(params["dist-type"]);

    switch (tType_){
        case 1:
            fpick = boost::bind(&icgenerator::montecarlo,this);
            break;
        case 2:
            fpick = boost::bind(&icgenerator::linear,this);
            break;
        case 3:
            LOG(FATAL) << "staged-linear is currently not implemented";
        default:
            LOG(FATAL) << "Bad choice for potential";
    }
}

void icgenerator::gen_ics(){
    initConditions_.resize(boost::extents[trajs_.size()][*(std::max_element(trajs_.begin(),trajs_.end()))]);
    (*this).fpick();
}

void icgenerator::ret_ics(boost::shared_ptr<ic_array> icPtr){
    *icPtr = initConditions_;
}

void icgenerator::linear(){}

void icgenerator::montecarlo(){

    base_generator_type generator(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<> uni_dist(-1,1);
    boost::variate_generator<base_generator_type&, boost::uniform_real<> > uni(generator, uni_dist);

    //double trajs = std::accumulate(trajs_.begin(),trajs_.end(),0);

    for (int i=0;i<trajs_.size();i++){
        double range = abs(ranges_[i].first-ranges_[i].second)*0.5;
        for (int j=0;j<trajs_[i];j++){
            initConditions_[i][j] = range*uni();
        }
    }

    for (int i=0;i<trajs_.size();i++){
        for (int j=0;j<trajs_[i];j++){
            std::cout << initConditions_[i][j];
        }
    }
}

