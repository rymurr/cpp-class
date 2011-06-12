#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Binner

#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/multi_array.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

//#include "integrator.hpp"
//#include "force.hpp"
#include "coords.hpp"
//#include "potential.hpp"
#include "icgen.hpp"
#include "timer.hpp"
#include "binner.hpp"
#include "trajs.hpp"
#include "integrator.hpp"
#include "multi_array_serial.hpp"
#include "config.hpp"

static double escapeRange = 1E-80;

BOOST_AUTO_TEST_SUITE(binner)

BOOST_AUTO_TEST_CASE(trajsInit){
#ifdef GFlags_FOUND
    google::InstallFailureSignalHandler();
#endif
    using namespace timer;
    using namespace classical;
    Timer::create();

    anyMap test;

    test["variance"] = 1.;
    test["no-build"] = true;
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
    test2["means"] = xx;
    test["int-type"] = 3;
    test["tinitial"] = 0.;
    test["tfinal"] = 100.;
    test["dt"] = 1.;
    test["potPtr"] = forceFactory(5);
    test["kinPtr"] = forceFactory(6);

    boost::shared_ptr<std::vector<double> > intX, intZ, intY, intW;
    TimerStart("main1");
    TimerStart("main1a");
    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Binner> initBin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(1, test, gen, x, bin, initBin);
    trx.runTraj();
    intX = bin->int1D(1);
    }
    TimerStop("main1a");
    TimerStart("main1b");
    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Binner> initBin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(2, test, gen, x, bin, initBin);
    trx.runTraj();
    intY = bin->int1D(1);
    }
    TimerStop("main1b");
    TimerStart("main1c");
    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Binner> initBin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(4, test, gen, x, bin, initBin);
    trx.runTraj();
    intZ = bin->int1D(1);
    }
    TimerStop("main1c");

    TimerStart("main1d");
    {
    boost::shared_ptr<icgenerator> gen = boost::shared_ptr<icgenerator>(new icgenerator(test,&test2));
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    boost::shared_ptr<Binner> bin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Binner> initBin = boost::shared_ptr<Binner>(new Binner(N,V));
    boost::shared_ptr<Integrator> x = boost::shared_ptr<Integrator>(new Integrator(test));
    Coords vals(xx);
    Trajs trx(5, test, gen, x, bin, initBin);
    trx.runTraj();
    intW = bin->int1D(1);
    }
    TimerStop("main1d");

    TimerStop("main1");

    for(std::size_t i=0;i<intX->size();++i){
        BOOST_CHECK_CLOSE(intY->operator[](i), intX->operator[](i),1E-4);
        BOOST_CHECK_CLOSE(intZ->operator[](i), intX->operator[](i),1E-4);
        std::cout << intX->operator[](i) << " " << intW->operator[](i) << std::endl;
        BOOST_CHECK_CLOSE(intW->operator[](i), intX->operator[](i),1E-5);
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
    test2["means"] = xx;

    icgenerator gen(test,&test2);
    std::vector<int> N(2,128);
    std::vector<double> V(2,5);
    Binner bin(N,V), bin2(N,V);
    Coords vals(xx);
    TimerStart("main2");
    for(int i=0;i<6250000;++i){
       gen.retIC(vals);
       double w = gen.retWeight(vals);
       //std::cout << w << std::endl;
       bin(vals,w);
    }
    {
    std::ofstream ofs("testB.dat");
    boost::archive::text_oarchive oa(ofs);
    oa << bin;
    }

    {
    std::ifstream ifs("testB.dat");
    boost::archive::text_iarchive ia(ifs);
    ia >> bin2;
    }
    boost::shared_ptr<std::vector<double> > intX(bin.int1D(1)), intY(bin2.int1D(1));
    for(std::size_t i=0;i<intX->size();++i){
        std::cout << intX->operator[](i) << std::endl;
        BOOST_CHECK_CLOSE(intX->operator[](i), intY->operator[](i), 1E-6);
    }
    TimerStop("main2");
    TimerReport();
}

BOOST_AUTO_TEST_CASE(array_serial){

    boost::multi_array<double,3> x(boost::extents[10][10][10]), y;

    for (double *xi=x.data();xi!=x.data()+x.num_elements();++xi){
         *xi = 4.;
    }
    {
    std::ofstream ofs("testMA.dat");
    boost::archive::text_oarchive oa(ofs);
    oa << x;
    }

    {
    std::ifstream ifs("testMA.dat");
    boost::archive::text_iarchive ia(ifs);
    ia >> y;
    }
    for (int i=0;i<10;i++){
        for (int j=0;j<10;j++){
            for (int k=0;k<10;k++){
                BOOST_CHECK_CLOSE(x[i][j][k],y[i][j][k],1E-6);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
