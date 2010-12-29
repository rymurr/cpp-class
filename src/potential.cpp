
#include "potential.hpp"

using boost::any_cast;

potential::potential(){}

potential::~potential(){}

potential::potential(anyMap in_param){

    double theta=any_cast<double>(in_param["theta-nuc"]);
    double phi=any_cast<double>(in_param["phi-nuc"]);
    double rnuc=any_cast<double>(in_param["rnuc"]);
    charges_=any_cast<std::vector<double> >(in_param["charges"]);
    alpha_=any_cast<double>(in_param["smoothing"]);
    int pot_choice = any_cast<int>(in_param["pot-type"]);
    switch (pot_choice){
        case 1:
            fpick = boost::bind(&potential::hatom,this,_1,_2,_3);
            z1_=0.;
            x1_=0.;
            y1_=0.;
            break;
        case 2:
            fpick = boost::bind(&potential::hmol,this,_1,_2,_3);
            x1_=rnuc*sin(theta)*cos(phi);
            y1_=rnuc*sin(theta)*sin(phi);
            z1_=rnuc*cos(theta);
            break;
        case 3:
            LOG(FATAL) << "GAMESS Input: Not Implemented!";
            break;
        case 4:
            LOG(FATAL) << "Numerical Input: Not Implemented!";
            break;
        default:
             LOG(FATAL) << "the choice of potential" + boost::lexical_cast<std::string>(pot_choice) + "is not valid, the code will exit now";
    }
}



