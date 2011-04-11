

#include "fields.hpp"
namespace classical {

int Field::tot_ = 0;

boost::shared_ptr<Field> Field::makeField(anyMap& map){

    using boost::any_cast;

    Field::tot_++;
    int id = Field::tot_;
    double omega = any_cast<std::vector<double> >(map["omega"])[id];
    double fwhm = any_cast<std::vector<double> >(map["fwhm"])[id];
    double ef = any_cast<std::vector<double> >(map["ef"])[id];
    double phi = any_cast<std::vector<double> >(map["ce"])[id];
    int pol = any_cast<std::vector<int> >(map["pol"])[id];
    double ti = any_cast<double>(map["tinitial"]);
    double tf = any_cast<double>(map["tfinal"]);
    double tmid = 0.5*abs(tf-ti);
    boost::shared_ptr<Field> fpick;

    int field_type = any_cast<int>(map["env"]);
    switch( field_type ){
        case 1:
            fpick = boost::shared_ptr<ConstField>(new ConstField(id, ef, omega, phi, pol));
            LOG(INFO) << "building Constant Field";
            break;
        case 2:
            fpick = boost::shared_ptr<StaticField>(new StaticField(id, ef, pol));
            LOG(INFO) << "building Static Field";
            break;
        case 3:
            fpick = boost::shared_ptr<GaussianField>(new GaussianField(id, ef, omega, phi, tmid, fwhm, pol));
            LOG(INFO) << "building Gaussian Field";
            break;
        case 4:
            fpick = boost::shared_ptr<SineSquareField>(new SineSquareField(id, ef, omega, phi, fwhm, pol));
            LOG(INFO) << "building Sine Squared Field";
            break;
        default:
            LOG(FATAL) << "the choice of envelope for the field" + boost::lexical_cast<std::string>(id) + "is wrong, the code will exit now";
    }
    return fpick;
}
}


