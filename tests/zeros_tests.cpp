#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Zeros
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <boost/any.hpp>

#include "zeros.hpp"
#include "potential.hpp"
#include "coords.hpp"

BOOST_AUTO_TEST_SUITE(zeroes)

    BOOST_AUTO_TEST_CASE(simple_zero){
        using namespace classical;
        anyMap test;
        std::vector<double> y(3,0),x;
        x.push_back(1.);
        test["charges"] = x;
        test["smoothing"] = 0.;
        test["rnuc"] = 0.;
        test["theta-nuc"] = 0.;
        test["phi-nuc"] = 0.;
        test["pot-type"] = 1;
        test["ip"] = 0.5;
        test["omega"] = x;
        test["ce"] = y;
        test["ef"] = x;
        test["fwhm"] = x;
        test["env"] = 1;
        test["tfinal"] = 100.;
        test["tinitial"] = 1.;
        y[0] = 2.;
        Coords<double> z(y);
 
        boost::shared_ptr<Potential> hatom(potentialFactory(test));
        boost::shared_ptr<Field> constant(fieldFactory(test,0));
        boost::shared_ptr<Potential> constantField(potentialFactory(constant));
        boost::shared_ptr<std::vector<boost::shared_ptr<Potential> > > pots(new std::vector<boost::shared_ptr<Potential> >);
        pots->push_back(constantField);
        pots->push_back(hatom);
        boost::shared_ptr<Potential> comboPot(potentialFactory(pots));
        BOOST_CHECK_CLOSE(comboPot->operator()(z,0), -0.5,1E-5);


    }

BOOST_AUTO_TEST_SUITE_END()
