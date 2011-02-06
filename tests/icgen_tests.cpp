#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ICGen
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include "icgen.hpp"

using boost::any_cast;


BOOST_AUTO_TEST_SUITE(icgen)

BOOST_AUTO_TEST_CASE(initSingleRandIC)
{

    std::vector<double> x;
    x.push_back(1.);
    x.push_back(0.);
    double y = 1;
    boost::shared_ptr<std::vector<double> > z = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    SingleRandIC testrand(x,y);
    
    for (int i =0;i<10;i++){
        testrand.RetVal(z);
        std::cout << (*z)[0] << " " << (*z)[1] << "\n";
    }


}

BOOST_AUTO_TEST_CASE(initicgen)
{

    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(1.);
    x.push_back(0.);
    y.push_back(2);
    y.push_back(2);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 1;
    icgenerator gen(test);
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[2][4]));
    gen.ret_ics(retArray);

    std::cout << "\n" << std::endl;
    for (int i=0;i<2;i++){
        for (int j=0;j<4;j++){
            std::cout << (*retArray)[i][j] << " ";
        }
    }

    std::cout << "\n" << std::endl;
    test["nobuild"] = true;
    icgenerator gen2(test);
    boost::shared_ptr<std::vector<double> > vals = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 2; i++)
            (*vals)[i]= 202;
    for (int i=0;i<4;i++){
        gen2.get_ic(vals);
        for (int j=0;j<2;j++){
            std::cout << (*vals)[j] << " ";
        }
    }
    std::cout << "\n" << std::endl;


}

BOOST_AUTO_TEST_CASE(lineartest)
{

    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(5);
    y.push_back(5);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 2;
    
    icgenerator gen(test);
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[2][5]));
    gen.ret_ics(retArray);

    std::cout << "\n" << std::endl;
    for (int i=0;i<2;i++){
        for (int j=0;j<5;j++){
            std::cout << (*retArray)[i][j] << " ";
        }
    }
    
    std::cout << "\n" << std::endl;
    test["nobuild"] = true;
    icgenerator gen2(test);
    boost::shared_ptr<std::vector<double> > vals = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 2; i++)
            (*vals)[i]= 202;
    for (int i=0;i<25;i++){
        gen2.get_ic(vals);
        for (int j=0;j<2;j++){
            std::cout << (*vals)[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(lineartest2)
{

    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(5);
    y.push_back(5);
    y.push_back(5);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 2;
    
    std::cout << "\n" << std::endl;
    test["nobuild"] = true;
    icgenerator gen2(test);
    boost::shared_ptr<std::vector<double> > vals = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 3; i++)
            (*vals)[i]= 202;
    for (int i=0;i<125;i++){
        gen2.get_ic(vals);
        for (int j=0;j<3;j++){
            std::cout << (*vals)[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(lineartest3)
{

    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(3);
    y.push_back(3);
    y.push_back(3);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 2;
    
    std::cout << "\n" << std::endl;
    icgenerator gen2(test);
    boost::shared_ptr<std::vector<double> > vals = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 3; i++)
            (*vals)[i]= 202;
    for (int i=0;i<27;i++){
        gen2.get_ic(vals);
        for (int j=0;j<3;j++){
            std::cout << (*vals)[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(serialize1)
{
    std::ofstream ofs("test.dat");
    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(5);
    y.push_back(5);
    y.push_back(5);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 2;
    
    test["nobuild"] = true;
    icgenerator gen2(test);
    {
        boost::archive::text_oarchive oa(ofs);
        oa << gen2;    
    }


    icgenerator gen;
    std::ifstream ifs("test.dat");
    boost::archive::text_iarchive ia(ifs);
    ia >> gen;


    boost::shared_ptr<std::vector<double> > vals = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    boost::shared_ptr<std::vector<double> > vals2 = boost::shared_ptr<std::vector<double> >(new std::vector<double>(x));
    for (int i = 0; i < 3; i++){
            (*vals)[i]= 202;
            (*vals2)[i]= 203;
    }
    for (int i=0;i<125;i++){
        gen.get_ic(vals);
        gen2.get_ic(vals2);
        for (int j=0;j<3;j++){
            BOOST_CHECK_CLOSE((*vals)[j],(*vals2)[j],1E-6);
        }
    }

}

BOOST_AUTO_TEST_CASE(serialize2)
{
    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(5);
    y.push_back(5);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 2;

    icgenerator gen2;
    icgenerator gen(test);

    {
    std::ofstream ofs("test2.dat");
    boost::archive::text_oarchive oa(ofs);
    oa << gen;
    }

    {
    std::ifstream ifs("test2.dat");
    boost::archive::text_iarchive ia(ifs);
    ia >> gen2;
    }
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[2][5]));
    boost::shared_ptr<boost::multi_array<double, 2> > retArray2=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[2][5]));
    gen2.ret_ics(retArray2);
    gen.ret_ics(retArray);

    std::cout << "\n" << std::endl;
    for (int i=0;i<2;i++){
        for (int j=0;j<5;j++){
            BOOST_CHECK_CLOSE((*retArray)[i][j],(*retArray2)[i][j],1E-6);
        }
    }
}

BOOST_AUTO_TEST_CASE(serialize_member)
{

    anyMap test;
    std::vector<double> x;
    std::vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(5);
    y.push_back(5);
    test["dims"] = y;
    test["means"] = x;
    test["variance"] = 1.;
    test["dist-type"] = 1;
    icgenerator gen(test);

    gen.save("text", "test3.dat");

    icgenerator gen2;

    gen2.load("text", "test3.dat");
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[2][25]));
    boost::shared_ptr<boost::multi_array<double, 2> > retArray2=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[2][25]));

    gen2.ret_ics(retArray2);
    gen.ret_ics(retArray);

    for (int i=0;i<2;i++){
        for (int j=0;j<25;j++){
            BOOST_CHECK_CLOSE((*retArray)[i][j],(*retArray2)[i][j],1E-6);
        }
    }

}

BOOST_AUTO_TEST_SUITE_END()

