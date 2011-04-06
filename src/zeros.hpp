
#ifndef _ZEROS_HPP_
#define _ZEROS_HPP_

#include <map>
#include <vector>
#include <iterator>

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include "customGlog.hpp"
#include "potential.hpp"
//#include "fields.hpp"
#include "coords.hpp"
//class Field;


namespace classical {

//typedef std::map<std::string,boost::any> anyMap;
typedef boost::shared_ptr<Potential> potPtr;
typedef std::vector<std::pair<double,double> > zeroPts;
typedef std::pair<double,double> dPair;

/*
struct OneDParams {
    vecP pots_;
    double ip_;
};

double OneDFunc(double x, void *params);
*/
void crossing(const std::vector<double>&, zeroPts&);

class RootFinder {
    public:
        virtual ~RootFinder(){};

        virtual Coords<double> retZeros() = 0;
};

class OneDRootFinder: public RootFinder {
    private:
        potPtr pots_;
        double ip_,t0_;
    public:
        OneDRootFinder(const potPtr pots,const double ip, const double t0): pots_(pots), ip_(ip), t0_(t0){};

        virtual Coords<double> retZeros();

        virtual ~OneDRootFinder(){};
};


class FindZeros {
    private:
        boost::shared_ptr<RootFinder> rootPtr_;

    public:
        FindZeros(const int type, const double ip, const double t0, const potPtr pots){
            switch(type){
                case 1:
                    LOG(INFO) << "the 1 dimensional root finder is being used";
                    rootPtr_ = boost::shared_ptr<OneDRootFinder>(new OneDRootFinder(pots, ip, t0));
                    break;
                default:
                    LOG(FATAL) << "bad choice for root finder, can not proceed";
                    
            }
        }

        Coords<double> retZeros(){
            return rootPtr_->retZeros();
        }
};

}
#endif
