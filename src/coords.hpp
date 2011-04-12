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
class Coords: boost::arithmetic<Coords<T>
              ,boost::arithmetic<Coords<T>, T
//              ,boost::indexable<Coords<T>,int,T&
//              ,boost::dereferenceable<Coords<T>, T*>
//            >
        >
    >
{
    private:
        ///the vector which holds the n-d point
        std::vector<T> x_;
    public:
        Coords(int n, T x): x_(n,x){};

/**
    Constructor which copies an input vector.

    @param[in] x vector to be copied
*/
        Coords(std::vector<T> &x): x_(x){};

/**
    Copy Constructor

    @param[in] rhs Coord to be copied
*/
        Coords(const Coords &rhs){x_ = rhs.x_;};

        std::size_t size(){return x_.size();};

/**
    norm of vector: sqrt of the sum of all elements squared

    @return returns the norm
*/
        T norm() const{
           T norm(this->square());
           return sqrt(norm);
        };

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

        T dotProd(const Coords &rhs){Coords x(*this * rhs); return x.sum();}
/**
    returns pointer to begining of stored vector. A bit hacky and I want to make
    a full iterator at some point but dont have time.

    @todo make this better and use iterator class

    @return pointer to begining of coordinate vector
*/
        typename std::vector<T>::const_iterator begin() const {return x_.begin();};

        //typename Coords<T>::iterator begin2() {return x_.begin();};

/**
    returns pointer past the end of stored vector. See Coords::begin()

    @todo make this better and use iterator class

    @return pointer past the end of coordinate vector
*/
        typename std::vector<T>::const_iterator end() const {return x_.end();};

/**
    random access element operator. does the same as vector []

    @param[in] i element to be accessed

    @return value of element i in vector
*/
        T& operator[](int n){return const_cast<T&>(x_[n]);};

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
        Coords operator+=(const Coords<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::plus<T>());
            return *this;
        };

        Coords operator+=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1+=rhs);
            return *this;
        };

        Coords operator-=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1-=rhs);
            return *this;
        };

        Coords operator*=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1*=rhs);
            return *this;
        };

        Coords operator/=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1/=rhs);
            return *this;
        };

/**
    subtration equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be subtracted from this

    @return returns this after rhs is subtracted from it
*/
        Coords operator-=(const Coords<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::minus<T>());
            return *this;
        };
/**
    multiply equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be multiplied by this

    @return returns this after rhs is multiplied by it
*/        
        Coords operator*=(const Coords<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::multiplies<T>());
            return *this;
        };
/**
    division equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be divided by this

    @return returns this after rhs is divided by it
*/        
        Coords operator/=(const Coords<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::divides<T>());
            return *this;
        };

        class iterator: public boost::iterator_facade<iterator, Coords<T>, std::random_access_iterator_tag> {
            private:
                //std::vector<T>::iterator cptr_;
                Coords<T> *cptr_;

            public:
                iterator(): cptr_(0){};

                explicit iterator(Coords<T>* p): cptr_(p){};

            private:

                friend class boost::iterator_core_access;

                void increment() { cptr_ = cptr_++;};

                void decrement() { cptr_ = cptr_--;};

                void advance(int n){ cptr_ = cptr_+=n;};

                void equal(iterator const &other) const{
                    return this->cptr_ == other.cptr_;
                }

                T& dereference() const {return *cptr_;};

                int distance_to(iterator const &other) const{
                    return this->cptr_ - other.cptr_;
                }


        };
};

/**
    norm function which calls member function on input

    @param[in] x input Coords on which norm is done

    @return norm of vector Coords
*/
template <class T>
//template <class T=double>
inline T norm(const Coords<T> &x){return x.norm();};

/**
    calls square member function on input, calculates sum of squares of vector

    @param[in] x input Coords on which square is done

    @return square of sums of vector
*/
//template <class T=double>
template <class T>
inline T square(const Coords<T> &x){return x.square();};

//template <class T=double>
template <class T>
inline T sum(const Coords<T>& x){return x.sum();};
}

#endif
