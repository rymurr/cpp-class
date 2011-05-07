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
    void mutliMapReduce(){};

};
}
