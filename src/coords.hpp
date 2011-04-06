// $Id$
/**
 * @file coords.hpp
 *
 * implementation for coords class, the class to represent an N-D vector
 * most important operators and functions are implemented here
 * operators are implemented using the boost::opreators library which
 * which decorates my class with all the other operators I didn't define.
 *
 * @brief implementation of coords class
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$

#ifndef COORDS_HPP_
#define COORDS_HPP_

#include <map>
#include <string>
#include <iterator>
#include <vector>

#include <gsl/gsl_math.h>
#include <boost/lambda/lambda.hpp>
#include <boost/operators.hpp>
//#include <boost/iterator/iterator_facade.hpp>
//#include <boost/shared_ptr.hpp>

//#include "customGlog.hpp"

namespace classical {

/**
 * This class defines an n-dimensional vector of coordinates. It can in general represent
 * any collection of elements as it is basically a vector with operators added. It was
 * designed to hold a point in an n-d phase space for my classical propagator
 *
 * @todo convert to template class so it can hold more than doubles
 *
*/
class Coords: boost::arithmetic<Coords,boost::indexable<Coords,int,double&,boost::dereferenceable<Coords, double*> > >{
    private:
        ///the vector which holds the n-d point
        std::vector<double> x_;
    public:
        Coords(int n, double x): x_(n,x){};

/**
    Constructor which copies an input vector.

    @param[in] x vector to be copied
*/
        Coords(std::vector<double> &x): x_(x){};

/**
    Copy Constructor

    @param[in] rhs Coord to be copied
*/
        Coords(const Coords &rhs){x_ = rhs.x_;};

/**
    norm of vector: sqrt of the sum of all elements squared

    @return returns the norm
*/
        double norm() const;

/**
    sum of all elements sqared

    @return returns square of all elements
*/
        double square() const;

        double sum() const;

        double dotProd(const Coords &rhs){Coords x(*this * rhs); return x.sum();}
/**
    returns pointer to begining of stored vector. A bit hacky and I want to make
    a full iterator at some point but dont have time.

    @todo make this better and use iterator class

    @return pointer to begining of coordinate vector
*/
        std::vector<double>::const_iterator begin() const {return x_.begin();};

/**
    returns pointer past the end of stored vector. See Coords::begin()

    @todo make this better and use iterator class

    @return pointer past the end of coordinate vector
*/
        std::vector<double>::const_iterator end() const {return x_.end();};

/**
    random access element operator. does the same as vector []

    @param[in] i element to be accessed

    @return value of element i in vector
*/
        double& operator[](int n){return const_cast<double&>(x_[n]);};

/**
    dereference operator

    @return reference to first element in vector
*/
        double& operator*() const {return const_cast<double&>(x_.front());};


/**
    addition equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be added to this

    @return returns this after rhs is added to it
*/
        Coords operator+=(const Coords&);
/**
    subtration equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be subtracted from this

    @return returns this after rhs is subtracted from it
*/
        Coords operator-=(const Coords&);
/**
    multiply equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be multiplied by this

    @return returns this after rhs is multiplied by it
*/        
        Coords operator*=(const Coords&);
/**
    division equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be divided by this

    @return returns this after rhs is divided by it
*/        
        Coords operator/=(const Coords&);
};

/*
class CoordIterator: public boost::iterator_facade<CoordIterator, Coords, boost::random_access_iterator_tag> {
    public:
        CoordIterator(): coord_(0), cptr_(0){};

        explicit CoordIterator(Coords* p): coord_(p), cptr_(p->begin()){};

    private:

        friend class boost::iterator_core_access;

        void increment() { coord_ = coord_->next();};

        void decrement() { coord_ = coord_->prev();};

        void advance(int n){ coord_ = coord_->adv(n);};

        void equal(Coords const &other) const{
            return this->coord_ == other.coord_;
        }

        Coords& dereference() const {return *coord_;};

        int distance_to(Coords const &other) const{
            
        }

        Coords* coord_;
        std::vector<double>::iterator cptr_;


};
*/

/**
    norm function which calls member function on input

    @param[in] x input Coords on which norm is done

    @return norm of vector Coords
*/
double norm(const Coords&);

/**
    calls square member function on input, calculates sum of squares of vector

    @param[in] x input Coords on which square is done

    @return square of sums of vector
*/
double square(const Coords&);

inline double sum(const Coords& x){return x.sum();};
}

#endif
