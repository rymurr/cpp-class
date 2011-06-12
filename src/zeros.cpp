
#include "zeros.hpp"

namespace classical{

Coords OneDRootFinder::retZeros(){
    std::vector<double> signs,coords;
    Coords r(3,0);
    r[0] = -100000.;
    signs.push_back(pots_->operator()(r,t0_));
    coords.push_back(r[0]);
    for (int i=0;i<100;++i){
        r[0]=-50. + i;
        signs.push_back(ip_+pots_->operator()(r,t0_));
        coords.push_back(r[0]);
    }
    r[0] = 100000;
    signs.push_back(ip_+pots_->operator()(r,t0_));
    coords.push_back(r[0]);
    zeroPts crossPts;
    crossing(signs, coords, crossPts);

    double zero=0.;
    PotDouble f(pots_, ip_, t0_);
    foreach(dPair d, crossPts){
        double retZero = findZero(d, f);
        if (std::abs(retZero) >= std::abs(zero))
            zero = retZero;
    }
    r[0] = zero;
    //std::cout << r << std::endl;
    return r;
}


double findZero(const dPair &x, PotDouble &f){
    boost::uintmax_t max_iter(10000), max_iter2(max_iter);
    boost::math::tools::eps_tolerance<double> epsTol(30);
    //double (*fnPtr)(double) = *f.target<double (*)(double)>();
    dPair y = boost::math::tools::toms748_solve<PotDouble,double,boost::math::tools::eps_tolerance<double> >(f, x.first, x.second, epsTol, max_iter2);
    if (max_iter2 < max_iter){
        return 0.5*(y.first+y.second);
    } else {
        LOG(WARNING) << "root not found for bracket: " << x.first << " " << x.second << " this needs to be checked!";
        return 0.5*(x.first+x.second);
    }
}

void crossing(const std::vector<double> &x, const std::vector<double> &y, zeroPts &pairs){
    for (std::size_t i=0;i<x.size()-1;++i){
        if(x[i]*x[i+1]<0){
            pairs.push_back(std::make_pair(y[i],y[i+1]));
        }
    }
}
     
}
