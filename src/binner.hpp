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


namespace classical {
    class Binner;
}
namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive & ar, const classical::Binner * t, const unsigned int file_version);
}}


namespace classical{
typedef std::map<std::string,boost::any> anyMap;
typedef boost::multi_array<double,3> binState;
typedef boost::multi_array<double,2> binSlice;
typedef boost::shared_ptr<boost::multi_array<double,2> > binSlicePtr;
typedef std::vector<double> binSlice2;
typedef boost::shared_ptr<binSlice2 > binSlice2Ptr;

class Binner: boost::arithmetic<Binner> {
    private:
        binState bins_;
        Coords ranges_,dxs_;
        std::vector<int> Ns_;
        int size_;

        friend class boost::serialization::access;
        template<class Archive> friend void boost::serialization::save_construct_data(Archive & ar, const Binner * t, const unsigned int file_version);


        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & Ns_;
            ar & size_;
            ar & ranges_;
            ar & dxs_;
            ar & bins_;
        }

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
        //Binner(){LOG(ERROR) << "I shouldn't be called!"; };

        /*Binner(const Binner &x){
            size_ = x.size_;
            dxs_ = x.dxs_;
            ranges_ = x.ranges_;
            Ns_ = x.Ns_;
            this->resize();
            ///TODO: need to copy data here
            LOG(INFO) << "copy building binner";
        }*/

        Binner(std::vector<int>& Ns, std::vector<double>& xs):ranges_(xs), Ns_(Ns){
            size_ = Ns.size();
            dxs_ = Coords(size_);
            for (int i=0;i<size_;++i){
                dxs_[i] = 2.*ranges_[i]/Ns_[i];
            }
            this->resize();
            LOG(INFO) << "building binner";
        };

        Binner(std::vector<int>& Ns, Coords& xs):ranges_(xs), Ns_(Ns){
            size_ = Ns.size();
            dxs_ = Coords(size_);
            for (int i=0;i<size_;++i){
                dxs_[i] = 2.*ranges_[i]/Ns_[i];
            }
            this->resize();
            LOG(INFO) << "building binner";
        };

        /*
        Binner& operator=(const Binner& rhs){
            return rhs;
        };*/

        int size(){return size_;};

        std::vector<int> shape(){return Ns_;};

        Binner operator+=(const Binner& x){
            ///TODO: change if to assert or glog assert
            std::cout << bins_.shape()[0] << x.bins_.shape()[0] << bins_.shape()[1] << x.bins_.shape()[1] << bins_.shape()[2] << x.bins_.shape()[2] << std::endl;
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

        void operator()(Coords& x, double w, int in=0){
            boost::array<binState::index,3> i = {{binState::index(0),binState::index(0),binState::index(0)}};
            for (int j=0;j<size_;++j){
                if (fabs(x[j+in])>ranges_[j]) continue;
                i[j] = floor((x[j+in]+ranges_[j])/dxs_[j]);
                //std::cout << i[j] << " " << x[j] << " " << dxs_[j] << " " << ranges_[j] << std::endl;
            }
            //std::cout << i[0] << i[1] << i[2] << std::endl;
            bins_(i) += w;
        };

        boost::shared_ptr<binState> int3D(){
            ///TODO: should make this a const pointer or something, no need to copy the whole array!
            return boost::shared_ptr<binState>(new binState(bins_));
        }

        binSlice2Ptr range(int n){
            binSlice2Ptr x = binSlice2Ptr(new binSlice2(Ns_[n-1],-ranges_[n-1] + 0.5*dxs_[n-1]));

            for(int i=1;i<Ns_[n-1];++i){
                x->operator [](i)  += i*dxs_[n-1];
            }
            return x;
        };

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
            binSlicePtr retArray = binSlicePtr(new binSlice(boost::extents[x-1][y-1]) );

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

namespace boost { namespace serialization {
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
