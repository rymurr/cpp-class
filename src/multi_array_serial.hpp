#ifndef _MARRAY_SERIALIZE_HPP_
#define _MARRAY_SERIALIZE_HPP_


#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>
#include <boost/multi_array.hpp>


namespace boost { namespace serialization {
template <class Archive>
void save(Archive &ar, const boost::multi_array<double,3> &ma, const unsigned int version)
{
        ar << ma.shape()[0];
        ar << ma.shape()[1];
        ar << ma.shape()[2];
        for ( const double *x=ma.data();x!=ma.data()+ma.num_elements();++x){
            ar << *x;
        }
}
template <class Archive>
void load(Archive &ar, boost::multi_array<double,3> &ma, const unsigned int version)
{
        std::size_t i,j,k;
        ar >> i;
        ar >> j;
        ar >> k;
        ma.resize(boost::extents[i][j][k]);
        for (double *x=ma.data();x!=ma.data()+ma.num_elements();++x){
            ar >> *x;
        }
}
template <class Archive>
void serialize(Archive &ar, boost::multi_array<double,3> &ma, const unsigned int version)
{
        boost::serialization::split_free(ar, ma, version);
}

template <class Archive>
void save(Archive &ar, const boost::multi_array<double,2> &ma, const unsigned int version)
{
        ar << ma.shape()[0];
        ar << ma.shape()[1];
        for ( const double *x=ma.data();x!=ma.data()+ma.num_elements();++x){
            ar << *x;
        }
}
template <class Archive>
void load(Archive &ar, boost::multi_array<double,2> &ma, const unsigned int version)
{
        std::size_t i,j;
        ar >> i;
        ar >> j;
        ma.resize(boost::extents[i][j]);
        for (double *x=ma.data();x!=ma.data()+ma.num_elements();++x){
            ar >> *x;
        }
}
template <class Archive>
void serialize(Archive &ar, boost::multi_array<double,2> &ma, const unsigned int version)
{
        boost::serialization::split_free(ar, ma, version);
}

}
}

#endif
