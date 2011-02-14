

#include "fields.hpp"
namespace classical {

using boost::any_cast;

field::field(anyMap map, int id){

    id_=id;
    omega_ = any_cast<std::vector<double> >(map["omega"])[id];
    fwhm_ = any_cast<std::vector<double> >(map["fwhm"])[id];
    ef_ = any_cast<std::vector<double> >(map["ef"])[id];
    phi_ = any_cast<std::vector<double> >(map["ce"])[id];
    double ti = any_cast<double>(map["tinitial"]);
    double tf = any_cast<double>(map["tfinal"]);
    tmid_ = 0.5*abs(tf-ti);

    int field_type = any_cast<int>(map["env"]);
    switch( field_type ){
        case 1:
            fpick = boost::bind(&field::fconst,this,_1);
            break;
        case 2:
            fpick = boost::bind(&field::fstatic,this,_1);
            break;
        case 3:
            fpick = boost::bind(&field::gaussian,this,_1);
            break;
        case 4:
            fpick = boost::bind(&field::ssquare,this,_1);
            break;
        default:
            LOG(FATAL) << "the choice of envelope for the field" + boost::lexical_cast<std::string>(id) + "is wrong, the code will exit now";
    }
}

field::field(){}

field::~field(){}

}

