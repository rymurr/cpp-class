
#include "potential.hpp"

namespace classical{



boost::shared_ptr<Potential> Potential::makePotential(anyMap& in_param){

    using boost::any_cast;
    double theta=any_cast<double>(in_param["theta-nuc"]);
    double phi=any_cast<double>(in_param["phi-nuc"]);
    double rnuc=any_cast<double>(in_param["rnuc"]);
    std::vector<double> charges=any_cast<std::vector<double> >(in_param["charges"]);
    double alpha=any_cast<double>(in_param["smoothing"]);
    int pot_choice = any_cast<int>(in_param["pot-type"]);
    boost::shared_ptr<Potential> fpick;
    std::vector<double> x;
    x.push_back(rnuc*sin(theta)*cos(phi)*0.5);
    x.push_back(rnuc*sin(theta)*sin(phi)*0.5);
    x.push_back(rnuc*cos(theta)*0.5);
    Coords<double> y(x);

    switch (pot_choice){
        case 1:
            fpick = boost::shared_ptr<HAtomPotential>(new HAtomPotential(charges[0], alpha));
            break;
        case 2:
            fpick = boost::shared_ptr<HMolPotential>(new HMolPotential(y, alpha, charges[0], charges[1]));
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
    return fpick;
}


}
