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

//#include <map>
//#include <string>
#include <iterator>
#include <vector>

//#include <gsl/gsl_math.h>
#include <math.h>
#include <boost/lambda/lambda.hpp>
#include <boost/operators.hpp>
#include <boost/iterator/iterator_facade.hpp>
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
template <class T>
//template <class T=double>
class Point: boost::arithmetic<Point<T>
              ,boost::arithmetic<Point<T>, T
              ,boost::indexable<Point<T>,std::size_t,T&
              ,boost::dereferenceable<Point<T>, T*>
            >
        >
    >
{
    private:
        ///the vector which holds the n-d point
        std::vector<T> x_;
    public:
        Point(){};

        Point(int n, T x): x_(n,x){};

        class const_iterator: public boost::iterator_facade<const_iterator, T, boost::random_access_traversal_tag >{
            private:
                typename std::vector<T>::iterator iter_;

                friend class boost::iterator_core_access;

                void increment() { ++iter_;}

                void decrement() { --iter_; }

                void advance(std::size_t n){ iter_+=n;};

                bool equal(const_iterator const &other) const{
                    return this->iter_ == other.iter_;
                }

                const T& dereference() const {return *iter_;};

                int distance_to(const_iterator const &other) const{
                    return this->iter_ - other.iter_;
                }

                public:
                    const_iterator():iter_(0){};

                    const_iterator(const const_iterator& it):iter_(it.iter_){};
                    const_iterator(const_iterator& it):iter_(it.iter_){};

                    explicit const_iterator(const typename std::vector<T>::iterator& it):iter_(it){};

        };

        class iterator: public boost::iterator_facade<iterator, T, boost::random_access_traversal_tag >{
            private:
                typename std::vector<T>::iterator iter_;

                friend class boost::iterator_core_access;

                void increment() { ++iter_;}

                void decrement() { --iter_; }

                void advance(std::size_t n){ iter_+=n;};

                bool equal(iterator const &other) const{
                    return this->iter_ == other.iter_;
                }

                T& dereference() const {return *iter_;};

                int distance_to(iterator const &other) const{
                    return this->iter_ - other.iter_;
                }

                public:
                    iterator():iter_(0){};

                    iterator(const iterator& it):iter_(it.iter_){};
                    iterator(iterator& it):iter_(it.iter_){};

                    explicit iterator(const typename std::vector<T>::iterator& it):iter_(it){};

            };


/**
    Constructor which copies an input vector.

    @param[in] x vector to be copied
*/
        Point(std::vector<T> &x): x_(x){};

/**
    Copy Constructor

    @param[in] rhs Coord to be copied
*/
        Point(const Point &rhs){x_ = rhs.x_;};

        Point(int n): x_(n){};

/**
    norm of vector: sqrt of the sum of all elements squared

    @return returns the norm
*/
        T norm() const{
           T norm(this->square());
           return sqrt(norm);
        };

        T abs() const{return this->norm();};

/**
    sum of all elements sqared

    @return returns square of all elements
*/
        T square() const{
            T square(0);
            std::for_each(x_.begin(),x_.end(),square+=boost::lambda::_1*boost::lambda::_1);
            return square;
        };

        T sum() const{
            T sum(0);
            std::for_each(x_.begin(),x_.end(),sum+=boost::lambda::_1);
            return sum;
        };

        T dotProd(const Point &rhs){Point x(*this * rhs); return x.sum();}
/**
    returns pointer to begining of stored vector. A bit hacky and I want to make
    a full iterator at some point but dont have time.

    @todo make this better and use iterator class

    @return pointer to begining of coordinate vector
*/

        iterator begin() {return iterator(x_.begin());};
        const_iterator begin() const {return const_iterator(x_.begin());};

        //typename Coords<T>::iterator begin2() {return x_.begin();};

/**
    returns pointer past the end of stored vector. See Coords::begin()

    @todo make this better and use iterator class

    @return pointer past the end of coordinate vector
*/
        iterator end() {return iterator(x_.end());};
        const_iterator end() const {return const_iterator(x_.end());};

/**
    random access element operator. does the same as vector []

    @param[in] i element to be accessed

    @return value of element i in vector
*/
        T& operator[](std::size_t n) const {return const_cast<double&>(x_[n]);};
        T& operator*() const {return *x_;};

        std::size_t size() const {return x_.size();};
/**
    dereference operator

    @return reference to first element in vector
*/
        //T& operator*() const {return const_cast<T&>(x_.front());};


/**
    addition equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be added to this

    @return returns this after rhs is added to it
*/
        Point operator+=(const Point<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::plus<T>());
            return *this;
        };

        Point operator+=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1+=rhs);
            return *this;
        };

        Point operator-=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1-=rhs);
            return *this;
        };

        Point operator*=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1*=rhs);
            return *this;
        };

        Point operator/=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1/=rhs);
            return *this;
        };

/**
    subtration equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be subtracted from this

    @return returns this after rhs is subtracted from it
*/
        Point operator-=(const Point<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::minus<T>());
            return *this;
        };
/**
    multiply equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be multiplied by this

    @return returns this after rhs is multiplied by it
*/        
        Point operator*=(const Point<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::multiplies<T>());
            return *this;
        };
/**
    division equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be divided by this

    @return returns this after rhs is divided by it
*/        
        Point operator/=(const Point<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::divides<T>());
            return *this;
        };

};

/**
    norm function which calls member function on input

    @param[in] x input Coords on which norm is done

    @return norm of vector Coords
*/
template <class T>
//template <class T=double>
inline T norm(const Point<T> &x){return x.norm();};

/**
    calls square member function on input, calculates sum of squares of vector

    @param[in] x input Coords on which square is done

    @return square of sums of vector
*/
//template <class T=double>
template <class T>
inline T square(const Point<T> &x){return x.square();};

//template <class T=double>
template <class T>
inline T sum(const Point<T>& x){return x.sum();};

typedef Point<double> Coords;
}

#endif
