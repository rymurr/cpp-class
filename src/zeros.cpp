
#include "zeros.hpp"

namespace classical{
/*
double OneDFunc(double x, void *params){
    struct OneDParams *p = static_cast<OneDParams*>(params);

    vecP pots = p->pots_;
    double ip = p->ip_;
    std::vector<double> y(3,0);
    y[2] = x;
    Coords z(y);
    std::for_each(pots->begin(),pots->end(),ip+=boost::lambda::bind(&Potential::operator(),*boost::lambda::_1,z));
    return ip;
}
*/

Coords<double> OneDRootFinder::retZeros(){
    std::vector<double> signs;
    std::vector<double> r(3,0);
    r[2] = -100000.;
    signs.push_back(ip_+pots_->operator()(r,t0_));
    for (int i=0;i<100;++i){
        r[2]=-50. + 10.*i;
        signs.push_back(ip_+pots_->operator()(r,t0_));
    }
    r[2] = 100000;
    signs.push_back(ip_+pots_->operator()(r,t0_));
    zeroPts crossPts;
    crossing(signs, crossPts);
    std::for_each(crossPts.begin(),crossPts.end(),std::cout << boost::lambda::bind(&std::pair<double,double>::first,boost::lambda::_1));
    return r;
}

void crossing(const std::vector<double> &x, zeroPts &pairs){
    //TODO: change into an algorithm and use lambda, bind etc.
    for (unsigned int i=0;i<x.size()-1;++i){
        if(x[i]*x[i+1]<0){
            pairs.push_back(std::make_pair(x[i],x[i+1]));
        }
    }
}
     
/*
Coords OneDRootFinder::retZeros(){
    const gsl_root_fsolver_type *T;
    gsl_root_fsolver *s;

    gsl_function F;
    struct OneDParams params = {pots_, ip_};

    F.function = &OneDFunc;
    F.params = &params;

    T = gsl_root_fsolver_brent;
    s = gsl_root_fsolver_alloc(T);
    double x_lo(1.0), x_hi(1E6);
    gsl_root_fsolver_set(s, &F, x_lo, x_hi);

    int iter(0), max_iter(100000), status;
    double r;
    do {
        ++iter;
        status = gsl_root_fsolver_iterate(s);
        r = gsl_root_fsolver_root(s);
        x_lo = gsl_root_fsolver_x_lower(s);
        x_hi = gsl_root_fsolver_x_upper(s);
        status = gsl_root_test_interval(x_lo, x_hi, 0, 1E-6);
    } while (status == GSL_CONTINUE && iter < max_iter);

    gsl_root_fsolver_free(s);

    if (status == GSL_SUCCESS){
        LOG(INFO) << "Zero found!";
    } else {
        LOG(INFO) << "Zero not found! returning closest value.";
    }
    std::vector<double> y(3,0);
    y[3] = r;
    Coords x(y);
    return x;
}
*/
}
