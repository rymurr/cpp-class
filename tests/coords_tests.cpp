#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Coords
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "coords.hpp"

BOOST_AUTO_TEST_SUITE(coords)

BOOST_AUTO_TEST_CASE(init){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x);
    Coords z(y);

    for(Coords::iterator i=y.begin();i!=y.end();++i){
        BOOST_REQUIRE_EQUAL(5,*i);
    }

    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(5,*i);
    }

    z+=y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(10,*i);
    }

}

BOOST_AUTO_TEST_CASE(optestplus){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x), z(x);

    z = z+y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(10,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestminus){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x), z(x);

    z = z-y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(0,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestminus2){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x), z(x);

    z -= y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(0,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestmult){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x), z(x);

    z = z*y;
    z *= y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(125,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestdiv){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x), z(x);

    z = z/y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(1,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestdiv2){
    using namespace classical;
    std::vector<double> x(3,5);
    Coords y(x), z(x);

    z /= y;
    for(Coords::iterator i=z.begin();i!=z.end();++i){
        BOOST_REQUIRE_EQUAL(1,*i);
    }
}

BOOST_AUTO_TEST_CASE(optestplus3){
    using namespace classical;
    double y(3.);
    Coords x(3,3);
    x=x+y;
    x+=y;
    x=x-y;
    x-=y;
    for(Coords::iterator i=x.begin();i!=x.end();++i){
        BOOST_REQUIRE_EQUAL(3.,*i);
    }
}


BOOST_AUTO_TEST_CASE(optestplus4){
    using namespace classical;
    double y(3.);
    Coords x(3,3);
    x=x*y;
    x*=y;
    x=x/y;
    x/=y;
    for(Coords::iterator i=x.begin();i!=x.end();++i){
        BOOST_REQUIRE_EQUAL(3.,*i);
    }
}

BOOST_AUTO_TEST_CASE(normsquare){
    using namespace classical;
    std::vector<double> x(3,2);
    Coords y(x);

    BOOST_REQUIRE_EQUAL(12,square(y));
    BOOST_REQUIRE_EQUAL(12,square(Coords(x)));
    BOOST_REQUIRE_EQUAL(sqrt(12),norm(y));
}

BOOST_AUTO_TEST_CASE(indexable){
    using namespace classical;
    std::vector<double> x(3,2);
    Coords y(x);

    BOOST_REQUIRE_EQUAL(2,x[0]);
    x[1] = 5;
    BOOST_REQUIRE_EQUAL(5,x[1]);
}

BOOST_AUTO_TEST_CASE(iterating){
    using namespace classical;
    Coords x(100,100.);

    for(Coords::iterator i=x.begin();i!=x.end();i++){
       BOOST_REQUIRE_EQUAL(*i,100.);
    }

}

BOOST_AUTO_TEST_CASE(serialize){
    using namespace classical;
    Coords x(10,10), y;

    {
        std::ofstream ofs("testC.dat");
        boost::archive::text_oarchive oa(ofs);
        oa << x;
    }

    {
        std::ifstream ifs("testC.dat");
        boost::archive::text_iarchive ia(ifs);
        ia >> y;
    }
    for (int i=0;i<10;++i){
        BOOST_CHECK_CLOSE(x[i],y[i],1E-6);
    }
}
BOOST_AUTO_TEST_SUITE_END()
