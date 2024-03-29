#ifndef ICSINGLE_H_
#define ICSINGLE_H_

#include <map>
#include <string>
#include <iostream>
#include <vector>

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/foreach.hpp>

#include "coords.hpp"
#include "customGlog.hpp"
#include "exceptions.hpp"

namespace classical{

#define foreach     BOOST_FOREACH

typedef std::map<std::string,boost::any> anyMap;

// This is a typedef for a random number generator.
// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
//try boost::rand48 for quickest/least accurate
//TODO: may want to wrap in #define or something to choose for release/debug...profile first for large rands
typedef boost::minstd_rand base_generator_type;
//typedef boost::rand48 base_generator_type;

using boost::any_cast;

class SingleGrid {
    private:
        double mean_, var_, start_, finish_, dx_, current_;
        int size_;


    public:

        SingleGrid(){};

        SingleGrid(double mean, double var, int size): mean_(mean), var_(var), size_(size){
            start_ = mean_ - 4.*sqrt(var_);
            finish_ = mean_ + 4.*sqrt(var_);
            dx_ = (finish_ - start_)/((double) size_);
            current_ = start_ - 0.5*dx_;
            finish_ -= dx_;
        };

        std::pair<double,bool> operator()(){
            current_ += dx_;
            if (current_  >= finish_){
                double old = current_;
                current_ = start_ - 0.5*dx_;
                return std::make_pair(old,true);
            } else {
                return std::make_pair(current_,false);
            }
        };

};

class SingleIC{


    public:

        //TODO: make the following private, move them into derived classes
        double var_;

        std::vector<double> mean_;

        SingleIC(){};

        virtual ~SingleIC(){};

        SingleIC(std::vector<double> mean, double var): var_(var), mean_(mean){};

        void virtual operator()(Coords&){};

        void virtual reset(int){};

        void virtual seek(int){};

};

class SingleLinIC: public SingleIC{
    private:

        std::vector<std::pair<double,bool> > current_;
        std::vector<SingleGrid> grids_;
        std::vector<int> sizes_;
        int size_;

    public:

        SingleLinIC(){};

        ~SingleLinIC(){};

        SingleLinIC(std::vector<double> means, double var, std::vector<int> sizes): SingleIC(means,var), sizes_(sizes){
            size_ = (*this).mean_.size();
            current_.resize(size_);
            grids_.resize(size_);
            for(int i=0;i<(*this).size_;i++){
                grids_[i] = SingleGrid(means[i],var,sizes[i]);
                current_[i] = std::make_pair(0.,false);
            }
            std::transform(grids_.begin()+1,grids_.end(),current_.begin()+1,boost::lambda::bind(&SingleGrid::operator(),boost::lambda::_1));
        };

        void next(){
            int i=0;
            bool exit = false;
            while (!exit && i<(*this).size_){
                if (!current_[i].second)
                    exit = true;
                current_[i] = grids_[i]();
                ++i;
            }
        }

        virtual void operator()(Coords& retVal){
            next();
            std::transform(current_.begin(),current_.end(),retVal.begin(), boost::lambda::bind(&std::pair<double,bool>::first,boost::lambda::_1));
        };

        virtual void seek(int n){
            ///TODO: parallelize this
            for (int i=0;i<n;++i){
                next();
            }
        };

 };

class SingleRandIC: public SingleIC{
    private:
        boost::normal_distribution<> dist_;
        boost::shared_ptr<boost::variate_generator<base_generator_type&, boost::normal_distribution<> > >  generators_;
        base_generator_type generator_;
    public:
        SingleRandIC(){};

        ~SingleRandIC(){};

        SingleRandIC(std::vector<double> mean, double var): SingleIC(mean, var){
            generator_.seed(static_cast<unsigned int>(std::time(0)));
            dist_ = boost::normal_distribution<>(0,(*this).var_);
            generators_ = boost::shared_ptr<boost::variate_generator<base_generator_type&, boost::normal_distribution<> > >( new boost::variate_generator<base_generator_type&, boost::normal_distribution<> >(generator_,dist_));
        };

        void operator()(Coords& retVal){
            //std::transform(this->mean_.begin(),this->mean_.end(),retVal.begin(),boost::lambda::_1 + generators_->operator()());
            for (std::size_t i=0;i<mean_.size();++i){
                retVal[i] = mean_[i] + generators_->operator()();
            }
        };

        virtual void seek(int n){
            generators_->engine().seed(std::time(0)*n);
            generators_->distribution().reset();
        };
};


}
#endif
