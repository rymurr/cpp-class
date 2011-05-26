#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Binner

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>

//#include "integrator.hpp"
//#include "force.hpp"
#include "coords.hpp"
//#include "potential.hpp"
#include "icgen.hpp"
#include "timer.hpp"
#include "binner.hpp"
#include "trajs.hpp"
#include "integrator.hpp"

BOOST_AUTO_TEST_SUITE(binner)

BOOST_AUTO_TEST_CASE(trajsInit){
    google::InstallFailureSignalHandler();
    using namespace timer;
    using namespace classical;
    Timer::create();

    anyMap test;

    test["variance"] = 1.;
    std::vector<double> xx(4,0.);
    std::vector<int> yy(4,5);
    test["dims"] = yy;
    test["means"] = xx;
    test["nobuild"] = true;
    test["dist-type"] = 2;
    anyMap test2;
    test2["weight-func"] = 2;
    test2["ef"] = std::vector<double>(1,0.6666666666);
    test2["ip"] = 1.0;
    test2["sigmas"] = boost::shared_ptr<std::vector<double> >(new std::vector<double>(4,1.));
    test["int-type"] = 3;
    test["tinitial"] = 0.;
    test["tfinal"] = 100.;
    test["dt"] = 1.;
    test["potPtr"] = forceFactory(5);
    test["kinPtr"] = forceFactory(6);

    boost::shared_ptr<std::vector<double> > intX, intZ, intY;
    TimerStart("main1");

    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(1, test, gen, x, bin);
    trx.runTraj();
    intX = bin->int1D(1);
    }
    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(2, test, gen, x, bin);
    trx.runTraj();
    intY = bin->int1D(1);
    }

    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(4, test, gen, x, bin);
    trx.runTraj();
    intZ = bin->int1D(1);
    }
    TimerStop("main1");
    for(std::size_t i=0;i<intY->size();++i){
        std::cout << intY->operator[](i) << " " << intX->operator[](i) << " " << intZ->operator[](i) << std::endl;
    }


    TimerReport();

}

BOOST_AUTO_TEST_CASE(nullSample){
    using namespace timer;
    using namespace classical;
    Timer::create();

    anyMap test;

    test["variance"] = 1.;
    std::vector<double> xx(4,0.);
    std::vector<int> yy(4,50);
    test["dims"] = yy;
    test["means"] = xx;
    test["nobuild"] = true;
    test["dist-type"] = 1;
    anyMap test2;
    test2["weight-func"] = 2;
    test2["ef"] = std::vector<double>(1,0.6666666666);
    test2["ip"] = 1.0;
    test2["sigmas"] = boost::shared_ptr<std::vector<double> >(new std::vector<double>(4,1.));

    icgenerator gen(test,&test2);
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    Binner bin(N,V);
    Coords vals(xx);
    TimerStart("main2");
    for(int i=0;i<6250000;++i){
       gen.retIC(vals);
       double w = gen.retWeight(vals);
       //std::cout << w << std::endl;
       bin(vals,w);
    }
    boost::shared_ptr<std::vector<double> > intX(bin.int1D(1));
    for(std::size_t i=0;i<intX->size();++i){
        std::cout << intX->operator[](i) << std::endl;
    }
    TimerStop("main2");
    TimerReport();
}

BOOST_AUTO_TEST_SUITE_END()
