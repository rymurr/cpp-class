

#include "fields.hpp"
namespace classical {

boost::shared_ptr<Field> Field::makeField(anyMap map, int id){

    using boost::any_cast;

    double omega = any_cast<std::vector<double> >(map["omega"])[id];
    double fwhm = any_cast<std::vector<double> >(map["fwhm"])[id];
    double ef = any_cast<std::vector<double> >(map["ef"])[id];
    double phi = any_cast<std::vector<double> >(map["ce"])[id];
    double ti = any_cast<double>(map["tinitial"]);
    double tf = any_cast<double>(map["tfinal"]);
    double tmid = 0.5*abs(tf-ti);
    boost::shared_ptr<Field> fpick;

    int field_type = any_cast<int>(map["env"]);
    switch( field_type ){
        case 1:
            fpick = boost::shared_ptr<ConstField>(new ConstField(id, ef, omega, phi));
            break;
        case 2:
            fpick = boost::shared_ptr<StaticField>(new StaticField(id, ef));
            break;
        case 3:
            fpick = boost::shared_ptr<GaussianField>(new GaussianField(id, ef, omega, phi, tmid, fwhm));
            break;
        case 4:
            fpick = boost::shared_ptr<SineSquareField>(new SineSquareField(id, ef, omega, phi, fwhm));
            break;
        default:
            LOG(FATAL) << "the choice of envelope for the field" + boost::lexical_cast<std::string>(id) + "is wrong, the code will exit now";
    }
    return fpick;
}
}


