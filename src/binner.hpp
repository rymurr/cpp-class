// $Id$
/**
 * @file binner.hpp
 *
 * binner class. This class stores a 3D array which is a histogram of
 * incoming data points. It uses an O(1) algorithm to place incoming data in its proper bin
 *
 * @brief definition and implementation of binner class
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$

#ifndef _BINNER_HPP_
#define _BINNER_HPP_

#include <string>
#include <map>
#include <cmath>
#include <boost/any.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/operators.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
//#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>

#include "multi_array_serial.hpp"
#include "coords.hpp"
#include "customGlog.hpp"

///primary namespace for classical propagator code
namespace classical {
    class Binner;
}
namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive & ar, const classical::Binner * t, const unsigned int file_version);
}}

///primary namespace for classical propagator code
namespace classical{
///defines map of any containers
typedef std::map<std::string,boost::any> anyMap;
///3D array for histogram
typedef boost::multi_array<double,3> binState;
///2d view of histogram
typedef boost::multi_array<double,2> binSlice;
///pointer to 2d view
typedef boost::shared_ptr<boost::multi_array<double,2> > binSlicePtr;
///1d view of histogram
typedef std::vector<double> binSlice2;
///pointer to 1d view
typedef boost::shared_ptr<binSlice2 > binSlice2Ptr;


/**
 * This is the primary class in the binner algorithm.
 */
class Binner: boost::arithmetic<Binner> {
    private:
        ///The 3D array which holds the histogram
        binState bins_;
        ///The range in each dimension from -x to x
        Coords ranges_;
        ///The spacing between the bins
        Coords dxs_;
        ///The number of bins per dimension
        std::vector<int> Ns_;
        ///number of dimensions being considered (in case simulation is <3D)
        int size_;

        ///grants access to private data for serialization
        friend class boost::serialization::access;
        ///grants access to private data to method which constructs binner upon save/load
        template<class Archive> friend void boost::serialization::save_construct_data(Archive & ar, const Binner * t, const unsigned int file_version);

/**
 * Serialization method needed by boost::serialization.
 * This method serialises the binner using boost.
 *
 * @param[in-out] ar archive into which the class is serialized or from where it is deserialized
 * @param[in] version used to serialize class depending on version (not used)
 */
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            //ar & Ns_;
            //ar & size_;
            ar & ranges_;
            ar & dxs_;
            ar & bins_;
        }

        /**
         * resizes internal array, used on copy and construction
         */
        void resize() {
            switch(size_){
                case 3:
                    bins_.resize(boost::extents[Ns_[0]][Ns_[1]][Ns_[2]]);
                    break;
                case 2:
                    bins_.resize(boost::extents[Ns_[0]][Ns_[1]][1]);
                    break;
                case 1:
                    bins_.resize(boost::extents[Ns_[0]][1][1]);
                    break;
                default:
                    LOG(ERROR) << "Wrong number of dimensions, either input error or not implemented yet...will not be binning";
            }
        };

    public:

        /**
         * The primary constructor, should be the only one called. This sets internal variables and resizes array
         *
         * @param[in] Ns number of bins per dimension
         * @param[in] xs range of histogram in each direction
         */
        Binner(std::vector<int>& Ns, std::vector<double>& xs):ranges_(xs), Ns_(Ns){
            size_ = Ns.size();
            dxs_ = Coords(size_);
            for (int i=0;i<size_;++i){
                dxs_[i] = 2.*ranges_[i]/Ns_[i];
            }
            this->resize();
            LOG(INFO) << "building binner";
        };

        /**
         * Same as primary constructor but with a different signature to allow for xs to be vector or Coords
         *
         * @param[in] Ns number of bins per dimension
         * @param[in] xs range of histogram in each direction
         */
        Binner(std::vector<int>& Ns, Coords& xs):ranges_(xs), Ns_(Ns){
            size_ = Ns.size();
            dxs_ = Coords(size_);
            for (int i=0;i<size_;++i){
                dxs_[i] = 2.*ranges_[i]/Ns_[i];
            }
            this->resize();
            LOG(INFO) << "building binner";
        };

        /**
         * getter for size_ internal variable
         * @return number of dimensions being used
         */
        int size(){return size_;};

        /**
         * getter for Ns_
         * @return number of bins per dimension
         */
        std::vector<int> shape(){return Ns_;};

        /**
         * assign addition operator, adds two bins together. used in conjunction with boost::operators::adable
         * @param[in] x Binner to be added to this
         * @return returns this, binner after addition
         */
        Binner operator+=(const Binner& x){
            ///TODO: change if to assert or glog assert
            if (bins_.shape()[0] == x.bins_.shape()[0] && bins_.shape()[1] == x.bins_.shape()[1] && bins_.shape()[2] == x.bins_.shape()[2] ){
                for (int i=0;i<bins_.shape()[0];++i){
                    for (int j=0;j<bins_.shape()[1];++j){
                        for (int k=0;k<bins_.shape()[2];++k){
                            bins_[i][j][k] += x.bins_[i][j][k];
                        }
                    }
                }
            } else {
                LOG(FATAL) << "bins are not the same size and can't be added!";
            }
            return *this;
        }

        /**
         * method which performs the histogram binning. Inputs are used to find weight and position of input and added to rest of histogram in O(1) time
         *
         * @param[in] x input trajectory
         * @param[in] w weight
         * @param[in] in offset in x which should be binned. in classical propagator this accesses either position 0:n or momentum n+1:N elements for binning
         */
        void operator()(Coords& x, double w, int in=0){
            boost::array<binState::index,3> i = {{binState::index(0),binState::index(0),binState::index(0)}};
            for (int j=0;j<size_;++j){
                if (fabs(x[j+in])>ranges_[j]) continue;
                i[j] = floor((x[j+in]+ranges_[j])/dxs_[j]);
            }
            bins_(i) += w;
        };

        /**
         * returns new copy of internal array for manipulation by other programs
         * @return 3D array of internal data
         */
        boost::shared_ptr<binState> int3D(){
            ///TODO: should make this a const pointer or something, no need to copy the whole array!
            return boost::shared_ptr<binState>(new binState(bins_));
        }


        /**
         * returns range in dimension n. an array from -ranges[n] to ranges[n]
         * @param[in] n dimension along which to create range
         * @return 1d array of range from -x to x
         */
        binSlice2Ptr range(int n){
            binSlice2Ptr x = binSlice2Ptr(new binSlice2(Ns_[n-1],-ranges_[n-1] + 0.5*dxs_[n-1]));

            for(int i=1;i<Ns_[n-1];++i){
                x->operator [](i)  += i*dxs_[n-1];
            }
            return x;
        };

        /**
         * returns 2d array of data after integrating out 1 dimension
         * @param[in] x first dimension to keep
         * @param[in] y second dimension to keep
         * @return 2d array of resultant data
         */
        boost::shared_ptr<binSlice> int2D(int x, int y){
            const binState::size_type* dims = bins_.shape();
            std::size_t dim1;
            if (x+y==3){
                dim1 = dims[2];
            } else if (x+y==5) {
                dim1 = dims[0];
            } else if (x+y==4) {
                dim1 = dims[1];
            } else {
                LOG(FATAL) << "bad input for int1D";
            }
            binSlicePtr retArray = binSlicePtr(new binSlice(boost::extents[dims[x-1]][dims[y-1]]) );

            for (std::size_t k=0;k<dims[x-1];++k){
                for (std::size_t i=0;i<dims[y-1];++i){
                    (*retArray)[k][i] = 0;
                    for (std::size_t j=0;j<dim1;++j){
                        (*retArray)[k][i] += bins_[k][i][j];
                    }
                }
            }
            return retArray;
        };

        /**
         * returns 1d array of data after integrating out 2 dimensions.
         * @param[in] n dimension to keep after integrating
         * @return 1d array of integrated data
         */
        binSlice2Ptr int1D(int x){
            const binState::size_type* dims = bins_.shape();
            std::size_t dim1, dim2;
            if (x==1){
                dim1 = dims[1];
                dim2 = dims[2];
            } else if (x==2) {
                dim1 = dims[0];
                dim2 = dims[2];
            } else if (x==3) {
                dim1 = dims[0];
                dim2 = dims[1];
            } else {
                LOG(FATAL) << "bad input for int1D";
            }

            binSlice2Ptr retArray = binSlice2Ptr(new binSlice2(dims[x-1]));
            for (std::size_t k=0;k<dims[x-1];++k){
                for (std::size_t i=0;i<dim1;++i){
                    for (std::size_t j=0;j<dim2;++j){
                        (*retArray)[k] += bins_[k][i][j];
                    }
                }
            }
            return retArray;
        };
};
}


namespace boost { namespace serialization {
/**
 * archives binner
 * @param[out] ar archive to load from
 * @param[in] t binner to construct and fill
 * @param[in] optional version parameter
 */
template<class Archive>
inline void save_construct_data(
    Archive & ar, const classical::Binner * t, const unsigned int file_version
){
    // save data required to construct instance
    //std::for_each(t->Ns_.begin(), t->Ns_.end(),std::cout << " turd " << boost::lambda::_1); std::cout << std::endl;
    //std::for_each(t->ranges_.begin(), t->ranges_.end(),std::cout << " spunk " << boost::lambda::_1); std::cout << std::endl;
    ar << t->Ns_;
    ar << t->ranges_;
}}}

///boost namespace
namespace boost {
///serialization namespace
namespace serialization {


/**
 * constructs binner while loading it from Archive
 * @param[in] ar archive to load from
 * @param[out] t binner to construct and fill
 * @param[in] optional version parameter
 */
template<class Archive>
inline void load_construct_data(
    Archive & ar, classical::Binner * t, const unsigned int file_version
){
    // retrieve data from archive required to construct new instance
    std::vector<int> Ns;
    classical::Coords ranges;

    ar >> Ns;
    ar >> ranges;
    //std::for_each(Ns.begin(), Ns.end(),std::cout << " turd " << boost::lambda::_1); std::cout << std::endl;
    //std::for_each(ranges.begin(), ranges.end(),std::cout << " spunk " << boost::lambda::_1); std::cout << std::endl;
    // invoke inplace constructor to initialize instance of my_class
    ::new(t)classical::Binner(Ns,ranges);
}
}}

#endif
