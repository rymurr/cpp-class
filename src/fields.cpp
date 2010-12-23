

#include "fields.hpp"

field::field(anyMap map, int id){

    omega = any_cast<std::vector<double> >(map["omega"])[id];
    fwhm = any_cast<std::vector<double> >(map["fwhm"])[id];
    ef = any_cast<std::vector<double> >(map["ef"])[id];
    phi = any_cast<std::vector<double> >(map["ce"])[id];
    double ti = any_cast<double>(map["tinitial"]);
    double tf = any_cast<double>(map["tfinal"]);
    tmid = 0.5*abs(tf-ti);

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

double field::operator()(double t){
    return (*this).fpick(t);
}

double field::fconst(double t){
    return ef * cos(omega*t+phi);
}

double field::fstatic(double t){
    return ef;
}

double field::gaussian(double t){
    return fconst(t) * exp(-4.0*log(2.0)*(t-tmid)*(t-tmid)/(fwhm*fwhm));
}

double field::ssquare(double t){
    return fconst(t) * pow(sin(pi*t*0.5/fwhm),2);
}
