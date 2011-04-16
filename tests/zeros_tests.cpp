#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Zeros
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <map>
#include <string>
#include <boost/any.hpp>

#include "zeros.hpp"
#include "potential.hpp"
#include "coords.hpp"

BOOST_AUTO_TEST_SUITE(zeroes)

    BOOST_AUTO_TEST_CASE(crossings){
        using namespace classical;
        std::vector<double> x(4,3);
        x[1] = -3.;
        zeroPts y;
        crossing(x,x,y);
        BOOST_CHECK_CLOSE(y[0].first,3,10E-5);
        BOOST_CHECK_CLOSE(y[0].second,-3,10E-5);
        BOOST_CHECK_CLOSE(y[1].first,-3,10E-5);
        BOOST_CHECK_CLOSE(y[1].second,3,10E-5);
        BOOST_CHECK_EQUAL(y.size(),2u);
    }

    BOOST_AUTO_TEST_CASE(simple_zero){
        using namespace classical;
        anyMap test;
        std::vector<double> y(3,0),x;
        x.push_back(1.);
        test["charges"] = x;
        test["smoothing"] = 1E-6;
        test["rnuc"] = 0.;
        test["theta-nuc"] = 0.;
        test["phi-nuc"] = 0.;
        test["pot-type"] = 1;
        test["ip"] = 0.5;
        x[0] = 0.01;
        test["omega"] = x;
        test["ce"] = y;
        test["ef"] = x;
        test["fwhm"] = x;
        test["env"] = 1;
        test["tfinal"] = 100.;
        test["tinitial"] = 1.;
        test["pol"] = std::vector<int>(1,3);
        y[0] = 2.;
        Coords z(3,0.);
        z[0] =2.;
 
        boost::shared_ptr<Potential> hatom(potentialFactory(test));
        boost::shared_ptr<Field> constant(fieldFactory(test));
        boost::shared_ptr<Potential> constantField(potentialFactory(constant));
        boost::shared_ptr<std::vector<boost::shared_ptr<Potential> > > pots(new std::vector<boost::shared_ptr<Potential> >);
        pots->push_back(constantField);
        pots->push_back(hatom);
        boost::shared_ptr<Potential> comboPot(potentialFactory(pots));
        BOOST_CHECK_CLOSE(comboPot->operator()(z,0), -0.5,1E-5);
        FindZeros zeros(1,boost::any_cast<double>(test["ip"]),0.,comboPot);
        Coords fz = zeros.retZeros();
        BOOST_CHECK_CLOSE(fz[2],-47.9129,1E-4);
    }

BOOST_AUTO_TEST_SUITE_END()
