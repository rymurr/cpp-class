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


#include <iterator>
#include <vector>

#include <math.h>

#include <boost/lambda/lambda.hpp>
#include <boost/operators.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>

///primary namespace for classical propagator code
namespace classical {

/**
 * This class defines an n-dimensional vector of coordinates. It can in general represent
 * any collection of elements as it is basically a vector with operators added. It was
 * designed to hold a point in an n-d phase space for my classical propagator
 *
 *
*/

template <class T>
//template <class T=double>
class Point: boost::arithmetic<Point<T>
              ,boost::arithmetic<Point<T>, T
              ,boost::indexable<Point<T>,std::size_t,T&
              //,boost::dereferenceable<Point<T>, iterator<T> >
            >
        >
    >
{
    private:
        ///the vector which holds the n-d point
        std::vector<T> x_;

        ///grants access to private data
        friend class boost::serialization::access;

        /**
         * simple serialization function for boost::serialization
         *
         * @param[out] ar archive to load from
         * @param[in] optional version parameter
         */
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & x_;
        }

    public:

        ///defines an iterator for the Point class
        typedef typename std::vector<T>::iterator iterator;
        ///defines a const iterator for the Point class
        typedef typename std::vector<T>::const_iterator const_iterator;
        ///defines value_type to make it work with certain STL algorithms
        typedef T value_type;


/**
    Default Constructor
*/
        Point(){};

/**
    Constructor which takes a default size and initial value

    @param[in] n number of elements to reserve
    @param[in] x default value for n initial elements
*/
        Point(int n, T x): x_(n,x){};

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

        /**
         * Constructs n-length point
         * @param[in] n initial size of array
         */

        Point(int n): x_(n){};

/**
    norm of vector: sqrt of the sum of all elements squared

    @return returns the norm
*/
        T norm() const{
           T norm(this->square());
           return sqrt(norm);
        };

/**
    norm of vector: sqrt of the sum of all elements squared, different name for use with odeint

    @return returns the norm
*/
        T abs() const{return this->norm();};

/**
    max value of vector: sqrt of the sum of all elements squared, different name for use with odeint

    @return returns the max element
*/
        T max() const{return std::max_element(x_.begin(), x_.end());};

/**
    resize function. resizes the vector to n

    @param[in] n new size of Point
*/
        void resize(std::size_t n) {x_.resize(n);};
/**
    resize function. resizes the vector to n

    @param[in] n new size of Point
    @param[in] x default value of new elements
*/
        void resize(std::size_t n, T x) {x_.resize(n, x);};

/**
    sum of all elements sqared

    @return returns square of all elements
*/
        T square() const{
            T square(0);
            std::for_each(x_.begin(),x_.end(),square+=boost::lambda::_1*boost::lambda::_1);
            return square;
        };

/**
    sum of all elements

    @return returns square of all elements
*/
        T sum() const{
            T sum(0);
            std::for_each(x_.begin(),x_.end(),sum+=boost::lambda::_1);
            return sum;
        };

/**
    dot product of this with rhs

    @param[in] rhs 2nd element of dot product

    @return result of dot product
*/
        T dotProd(const Point &rhs){Point x(*this * rhs); return x.sum();}


/**
    begin returns an iterator to the first element of the internal vector

    @return iterator to first element
*/
        iterator begin() {return iterator(x_.begin());};

/**
    begin returns a const iterator to the first element of the internal vector

    @return const iterator to first element
*/
        const_iterator begin() const {return const_iterator(x_.begin());};


/**
    returns iterator past the end of stored vector. See Coords::begin()

    @return iterator past the end of coordinate vector
*/
        iterator end() {return iterator(x_.end());};
/**
    returns const iterator past the end of stored vector. See Coords::begin()

    @return const iterator past the end of coordinate vector
*/
        const_iterator end() const {return const_iterator(x_.end());};

/**
    random access element operator. does the same as vector []

    @param[in] n element to be accessed

    @return value of element i in vector
*/
        T& operator[](std::size_t n) const {return const_cast<double&>(x_[n]);};

/**
    size of internal vector

    @return returns size of Point
*/
        std::size_t size() const {return x_.size();};

/**
    addition equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be added to this

    @return returns this after rhs is added to it
*/
        Point operator+=(const Point<T> &rhs){
            std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::plus<T>());
            return *this;
        };

/**
    addition equals operator, used by boost::operators to build full set.
    @param[in] rhs the vector to be added to this

    @return returns this after rhs is added to it
*/
        Point operator+=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1+=rhs);
            return *this;
        };


/**
    subtration equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be subtracted from this

    @return returns this after rhs is subtracted from it
*/
        Point operator-=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1-=rhs);
            return *this;
        };

/**
    multiply equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be multiplied by this

    @return returns this after rhs is multiplied by it
*/
        Point operator*=(const T &rhs){
            std::for_each(this->x_.begin(),this->x_.end(), boost::lambda::_1*=rhs);
            return *this;
        };

/**
    division equals operator, used by boost::operators to build full set.

    @param[in] rhs the vector to be divided by this

    @return returns this after rhs is divided by it
*/
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

/**
    ostream operator overload, used to write out Point to file/terminal in one shot. with less writing.
*/
        friend std::ostream& operator <<(std::ostream& out, const Point& x){
            std::for_each(x.begin(), x.end(), out << " " << boost::lambda::_1);
            return out;
        }



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

/**
    calls sum member function on input, calculates sum of a vector

    @param[in] x performs sum on x

    @return result of sum calculation
*/
//template <class T=double>
template <class T>
inline T sum(const Point<T>& x){return x.sum();};

/**
    calls abs member function on input

    @param[in] x input on which abs is done

    @return returns abs of x
 */
template <class T>
inline T abs(const Point<T>& x){return x.abs();};

/**
    calls max member function on input, calculates max element of input

    @param[in] x input Point on which max is performed

    @return returns max element
 */
template <class T>
inline T max(const Point<T>& x){return x.max();};

///typedef to define explicitly Coords as double specialization of Point
typedef Point<double> Coords;
}

#endif
