#ifndef TRAJS_HPP_
#define TRAJS_HPP_

#include "config.h"
#ifdef MPI_FOUND
//#include <mpi.h>
#include <assert.h>
#include <boost/mpi.hpp>
//#include <boost/mpi/communicator.hpp>
//#include <boost/mpi/operations.hpp>
//#include <mrmpi/mapreduce.h>
//#include <mrmpi/keyvalue.h>
#endif

#ifdef OPENMP_FOUND
#include <omp.h>
#endif

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>

#include "icgen.hpp"
#include "integrator.hpp"
#include "binner.hpp"
#include "coords.hpp"
#include "customGlog.hpp"

namespace classical{
typedef boost::shared_ptr<Binner> bPtr;

class Trajs {
    private:
        std::vector<Coords> nums_;
        boost::shared_ptr<Binner> bin_;
        boost::shared_ptr<Integrator> int_;
        boost::shared_ptr<icgenerator> gen_;
        boost::function<void (Trajs&)> run_;
        std::size_t numTrajs_;
        int dims_;
        double t_;

    public:
        Trajs(int id, anyMap&, boost::shared_ptr<icgenerator> gen, boost::shared_ptr<Integrator> integrate, boost::shared_ptr<Binner> bin);
    
    void runTraj(){
        run_((*this));
    }

    void multiOMPNoStore();
    void singleNoStore();
    void singleStore();
    void singleNoBin();
    void multiMapReduce();

};

class binAdd{
    public:
    bPtr operator()(bPtr, const bPtr);
};

}


namespace boost { namespace mpi {

  template<>
  struct is_commutative<classical::binAdd, boost::shared_ptr<classical::Binner> >
    : mpl::true_ { };

} }

#endif
