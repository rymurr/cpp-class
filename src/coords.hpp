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
//#include <iterator>
//#include <vector>

//#include <gsl/gsl_math.h>
//#include <boost/lambda/lambda.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/blas.hpp>
//#include <boost/operators.hpp>
//#include <boost/iterator/iterator_facade.hpp>
//#include <boost/shared_ptr.hpp>

//#include "customGlog.hpp"

namespace classical {

typedef boost::numeric::ublas::vector<double> Coords;
/**
 * This class defines an n-dimensional vector of coordinates. It can in general represent
 * any collection of elements as it is basically a vector with operators added. It was
 * designed to hold a point in an n-d phase space for my classical propagator
 *
 *
*/

/*
template <class T>
boost::math::ublas::vector operator+=(const T &rhs){
    std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1+=rhs);
    return *this;
}

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
template <class T>
inline T norm(const Coords<T> &x){return x.norm();};

template <class T>
inline T square(const Coords<T> &x){return x.square();};

template <class T>
inline T sum(const Coords<T>& x){return x.sum();};
}
*/
}
#endif
