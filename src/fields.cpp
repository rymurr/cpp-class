

#include "fields.hpp"

field::field(pmap map, int id){

    omega = any_cast<std::vector<double> >(map.map["omega"])[id];
    fwhm = any_cast<std::vector<double> >(map.map["fwhm"])[id];
    ef = any_cast<std::vector<double> >(map.map["ef"])[id];
    phi = any_cast<std::vector<double> >(map.map["ce"])[id];

    int field_type = any_cast<int>(map.map["env"]);
    switch( field_type ){
        case 1:
            fpick = boost::bind(&field::fconst,this,_1);
            break;
        case 2:
            fpick = boost::bind(&field::fstatic,this,_1);
            break;
/*
        case 3:
            fpick = &field::gaussian;
            break;
        case 4:
            fpick = &field::ssquare;
            break;
        default:
            LOG(FATAL) << "the choice of envelope for the field" + boost::lexical_cast<std::string>(id) + "is wrong, the code will exit now";*/
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
    return ef*t;
}

double field::gaussian(double t){
    return fconst(t) * exp(-4.0*log(2.0)*t*t/(fwhm*fwhm));
}

double field::ssquare(double t){
    return fconst(t) * pow(sin(pi*t*0.5/fwhm),2);
}
