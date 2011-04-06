// $Id$
/**
 * @file coords.cpp
 *
 * source for coords class, the class to represent an N-D vector
 * most important operators and functions are implemented here
 *
 * @brief source for coords class
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$

#include "coords.hpp"

namespace classical{

double Coords::sum() const{
    using namespace boost::lambda;
    double sum(0);
    std::for_each(x_.begin(),x_.end(),sum+=_1);
    return sum;
}

double Coords::square() const{
    using namespace boost::lambda;
    double square(0);
    std::for_each(x_.begin(),x_.end(),square+=_1*_1);
    return square;
}

double Coords::norm() const{
    double norm(this->square());
    return sqrt(norm);
}

Coords Coords::operator+=(const Coords &rhs){
    std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::plus<double>());
    return *this;
}

Coords Coords::operator-=(const Coords &rhs){
    std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::minus<double>());
    return *this;
}

Coords Coords::operator*=(const Coords &rhs){
    std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::multiplies<double>());
    return *this;
}

Coords Coords::operator/=(const Coords &rhs){
    std::transform(rhs.x_.begin(),rhs.x_.end(),this->x_.begin(),this->x_.begin(), std::divides<double>());
    return *this;
}

double norm(const Coords &x){
    return x.norm();
}

double square(const Coords &x){
    return x.square();
}

}
