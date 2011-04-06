#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Coords
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/lambda/lambda.hpp>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include "coords.hpp"

BOOST_AUTO_TEST_SUITE(coords)

BOOST_AUTO_TEST_CASE(init){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x);
    Coords<double> z(y);

    for(std::vector<double>::const_iterator i=y.begin();i!=y.end();++i){
        BOOST_REQUIRE_EQUAL(5,*i);
    }

    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(5,*i);
    }

    z+=y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(10,*i);
    }

}

BOOST_AUTO_TEST_CASE(optestplus){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x), z(x);

    z = z+y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(10,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestminus){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x), z(x);

    z = z-y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(0,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestminus2){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x), z(x);

    z -= y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(0,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestmult){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x), z(x);

    z = z*y;
    z *= y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(125,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestdiv){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x), z(x);

    z = z/y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(1,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestdiv2){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords<double> y(x), z(x);

    z /= y;
    for(std::vector<double>::const_iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(1,*i);
    }
}

BOOST_AUTO_TEST_CASE(normsquare){
    using namespace classical;
    std::vector<double> x(3,2);
    Coords<double> y(x);

    BOOST_REQUIRE_EQUAL(12,square(y));
    BOOST_REQUIRE_EQUAL(12,square(Coords<double>(x)));
    BOOST_REQUIRE_EQUAL(sqrt(12),norm(y));
}

BOOST_AUTO_TEST_CASE(indexable){
    using namespace classical;
    std::vector<double> x(3,2);
    Coords<double> y(x);

    BOOST_REQUIRE_EQUAL(2,x[0]);
    x[1] = 5;
    BOOST_REQUIRE_EQUAL(5,x[1]);
}
BOOST_AUTO_TEST_SUITE_END()
