#ifndef PYWRAPPER_HPP_
#define PYWRAPPER_HPP_

#include "config.hpp"
#ifdef PYTHONLIBS_FOUND
#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/list.hpp>
#include <boost/python/def.hpp>
#include <boost/python/detail/wrap_python.hpp>
#include <numpy/arrayobject.h>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include <boost/lambda/lambda.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


#include "input_param.hpp"
#include "icgen.hpp"
#include "fields.hpp"
//#include "potential.hpp"
#include "zeros.hpp"
#include "coords.hpp"
#include "binner.hpp"
#include "integrator.hpp"
#include "trajs.hpp"
#include "force.hpp"


namespace classical{

static bool googInit = false;

class Potential;
//class Field;

#ifdef PYTHONLIBS_FOUND
template <class tPtr>
PyObject* pyConvert(tPtr& data){
    const binState::size_type* dims = data->shape();
    const int ndims = data->num_dimensions();
    ///TODO: this step is useless and should be switched to a cast
    long Adims[ndims];
    for (int i=0;i<ndims;++i){
        Adims[i] = dims[i];
    }

    PyArrayObject *retval = reinterpret_cast<PyArrayObject*>(boost::python::expect_non_null(PyArray_SimpleNewFromData(ndims,Adims, PyArray_DOUBLE, data->origin())));

    return PyArray_Return(retval);
}

template <>
inline PyObject* pyConvert(binSlice2Ptr& data){
    long dims[1]; dims[0] = data->size();

    PyArrayObject *retval = reinterpret_cast<PyArrayObject*>(boost::python::expect_non_null(PyArray_SimpleNewFromData(1,dims, PyArray_DOUBLE, data->data())));

    return PyArray_Return(retval);
}
#endif


#ifdef PYTHONLIBS_FOUND
using namespace boost::python;
#endif

class simulation{
    private:
        int argc_;
        std::vector<std::string> argv_;
        std::string fname_;
        pmap params_;
        boost::shared_ptr<anyMap> map_;
        std::vector<boost::shared_ptr<Field> > fieldVec_;
        boost::shared_ptr<std::vector<boost::shared_ptr<Potential> > > pots_;
        boost::shared_ptr<std::vector<boost::shared_ptr<Force> > > forces_;
        boost::shared_ptr<Potential> totPot_;
        boost::shared_ptr<Force> totForce_;
        boost::shared_ptr<Force> kinForce_;
        boost::shared_ptr<icgenerator> icgen_;
        boost::shared_ptr<Binner> bins_, initBins_;
        boost::shared_ptr<Integrator> int_;
        boost::shared_ptr<Trajs> trajs_;

        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar & argc_;
            ar & argv_;
            ar & fname_;
            ar & icgen_;
            ar & bins_;
            ar & initBins_;
        }

        template<class Archive>
        void load(Archive &ar, const unsigned int version){
            ar & argc_;
            ar & argv_;
            ar & fname_;
            ar & icgen_;
            ar & bins_;
            ar & initBins_;
            this->common_setup();
            this->potsNFields();
            int_ = boost::shared_ptr<Integrator>(new Integrator(*map_));
            trajs_ = boost::shared_ptr<Trajs>(new Trajs(1, *map_, icgen_, int_, bins_, initBins_));
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER()

        void common_setup(){
#ifdef GFlags_FOUND
            if (!googInit){
                google::InitGoogleLogging("cpp-class");
                google::InstallFailureSignalHandler();
                googInit = true;
            }
#endif
        };

        void initTrajsBins();

        void potsNFields();

    public:
#ifdef PYTHONLIBS_FOUND
        simulation(int argc, boost::python::list argv, std::string fname): argc_(argc), fname_(fname){
            std::size_t n = boost::python::len(argv);
            for (unsigned int i = 0; i < n; i++) {
                argv_.push_back(boost::python::extract<std::string>(argv[i]));
            }
            common_setup();
        };
#endif

        simulation(int argc, std::vector<std::string> argv, std::string fname): argc_(argc), argv_(argv), fname_(fname){
            common_setup();
        };

        simulation(std::string sType, std::string fName){
            this->load(sType, fName);
        }

        ~simulation(){
#ifdef GFlags_FOUND
            google::ShutdownGoogleLogging();
#endif
        };

        int param_init(){
            LOG(INFO) << "building parameter set from input data";
            int retVal = params_.read_params(fname_, argc_, argv_);
            if (retVal == 0){
                params_.print();
                map_ = params_.map_out();
                LOG(INFO) << "Parameters have been built";
                return 0;
            } else if (retVal == 10){
                return 10;
            } else {
                LOG(ERROR) << "Parameters build was unsuccessful";
                return 1;
            };
            params_.print();
            map_ = params_.map_out();
            LOG(INFO) << "Parameters have been built";
        };

        void save_config(std::string fname){
            params_.print(fname);
        }

        void print_config(){
            params_.print();
        }

        void ic_gen();

        void run_trajs();

        void run_bins();


        boost::shared_ptr<std::vector<double> > range(int n){
            return bins_->range(n);
        };

        boost::shared_ptr<std::vector<double> > rangeInit(int n){
            return initBins_->range(n);
        };

        boost::shared_ptr<const Binner> retBin(){return bins_;};
        boost::shared_ptr<const Binner> retBinInit(){return initBins_;};

        boost::shared_ptr<binSlice> int2D(int x, int y){return bins_->int2D(x,y);};
        binSlice2Ptr int1D(int x){return bins_->int1D(x);};
        boost::shared_ptr<binState> int3D(){return bins_->int3D();};

        boost::shared_ptr<binSlice> int2DInit(int x, int y){return initBins_->int2D(x,y);};
        binSlice2Ptr int1Dinit(int x){return initBins_->int1D(x);};
        boost::shared_ptr<binState> int3DInit(){return initBins_->int3D();};

#ifdef PYTHONLIBS_FOUND
        PyObject* rangePython(int n){
            binSlice2Ptr x = bins_->range(n);
            return pyConvert(x);
        }

        PyObject* rangeInitPython(int n){
            binSlice2Ptr x = initBins_->range(n);
            return pyConvert(x);
        }

        PyObject* int1DPython(int n){
            binSlice2Ptr x = bins_->int1D(n);
            return pyConvert(x);
        }

        PyObject* int1DPythonInit(int n){
            binSlice2Ptr x = initBins_->int1D(n);
            return pyConvert(x);
        }

        PyObject* int2DPython(int n, int m){
            boost::shared_ptr<binSlice> x = bins_->int2D(n,m);
            return pyConvert(x);
        }

        PyObject* int2DPythonInit(int n, int m){
            boost::shared_ptr<binSlice> x = initBins_->int2D(n,m);
            return pyConvert(x);
        }

        PyObject* int3DPython(){
            boost::shared_ptr<binState> x = bins_->int3D();
            return pyConvert(x);
        }

        PyObject* int3DPythonInit(){
            boost::shared_ptr<binState> x = initBins_->int3D();
            return pyConvert(x);
        }
#endif

        void save(std::string sType, std::string fName){

            std::string binary="binary", text="text";
            std::ofstream ofs(fName.c_str());

            LOG(INFO) << "Saving icgenerator in " + sType + " format. Archive is called " + fName;
            if (sType.compare(binary) == 0){
                boost::archive::binary_oarchive oa(ofs);
                oa << *this;
            } else if (sType.compare(text) == 0) {
                boost::archive::text_oarchive oa(ofs);
                oa << *this;
            } else {
                std::string throwArg="archive type must be binary or text not" + sType;
                throw archive_error() << err_info(throwArg);
            }

        }

        void load(std::string sType, std::string fName){

            std::string binary="binary", text="text";
            std::ifstream ifs(fName.c_str());

            LOG(INFO) << "Loading icgenerator from " + sType + " format. Archive is called " + fName;
            try{
                if (sType.compare(binary) == 0){
                    boost::archive::binary_iarchive ia(ifs);
                    ia >> *this;
                } else if (sType.compare(text) == 0) {
                    boost::archive::text_iarchive ia(ifs);
                    ia >> *this;
                } else {
                    std::string throwArg="archive type must be binary or text not" + sType;
                    throw archive_error() << err_info(throwArg);
                }
            } catch (std::exception &e){
                LOG(ERROR) << "Binary is of wrong type, " << e.what();
                throw;
            }
        }

};


}

#endif
