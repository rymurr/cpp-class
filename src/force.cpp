
#include "force.hpp"

namespace classical{



boost::shared_ptr<Force> Force::makeForce(anyMap& in_param){

    using boost::any_cast;
    double theta=any_cast<double>(in_param["theta-nuc"]);
    double phi = any_cast<double>(in_param["phi-nuc"]);
    double rnuc=any_cast<double>(in_param["rnuc"]);
    std::vector<double> charges=any_cast<std::vector<double> >(in_param["charges"]);
    double alpha=any_cast<double>(in_param["smoothing"]);
    int pot_choice = any_cast<int>(in_param["pot-type"]);
    boost::shared_ptr<Force> fpick;
    Coords y(3);
    y[0] = (rnuc*sin(theta)*cos(phi)*0.5);
    y[1] = (rnuc*sin(theta)*sin(phi)*0.5);
    y[2] = (rnuc*cos(theta)*0.5);

    switch (pot_choice){
        case 1:
            fpick = boost::shared_ptr<HAtomForce>(new HAtomForce(charges[0], alpha));
            LOG(INFO) << "buiding Hydrogen Atomic Potential";
            break;
        case 2:
            fpick = boost::shared_ptr<HMolForce>(new HMolForce(y, alpha, charges[0], charges[1]));
            LOG(INFO) << "building Hydrogen Molecular Potential";
            break;
        case 3:
            LOG(FATAL) << "GAMESS Input: Not Implemented!";
            break;
        case 4:
            LOG(FATAL) << "Numerical Input: Not Implemented!";
            break;
        case 5:
            fpick = boost::shared_ptr<NullForce>(new NullForce());
            LOG(INFO) << "null force is being used";
            break;
        case 6:
            fpick = boost::shared_ptr<KineticForce>(new KineticForce());
            LOG(INFO) << "using force variant for kinetic energy operator";
            break;
        default:
            LOG(FATAL) << "the choice of potential" + boost::lexical_cast<std::string>(pot_choice) + "is not valid, the code will exit now";
    }
    return fpick;
}


}
