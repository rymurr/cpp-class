#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ICGen
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include "icgen.hpp"
#include "ic_weights.hpp"
#include "timer.hpp"
namespace classical{

using boost::any_cast;

struct anymap{
    anyMap test;
    std::vector<double> x;
    std::vector<int> y;

    anymap(){
        x.push_back(1.);
        x.push_back(0.);
        y.push_back(2);
        y.push_back(2);
        test["dims"] = y;
        test["means"] = x;
        test["variance"] = 1.;
        test["dist-type"] = 1;
        test["weight-func"] = 1;
    }
};


BOOST_FIXTURE_TEST_SUITE(icgen, anymap)

BOOST_AUTO_TEST_CASE(initSingleRandIC)
{
    std::vector<double> x;
    x.push_back(1.);
    x.push_back(0.);
    double y = 1;
    Coords z(x);
    SingleRandIC testrand(x,y);
    
    for (int i =0;i<10;i++){
        testrand(z);
        std::cout << z[0] << " " << z[1] << "\n";
    }


}

BOOST_AUTO_TEST_CASE(initicgen)
{

    icgenerator gen(test);
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[4][2]));
    gen.retICs(retArray);

    std::cout << "\n" << std::endl;
    for (int i=0;i<4;i++){
        for (int j=0;j<2;j++){
            std::cout << (*retArray)[i][j] << " ";
        }
    }

    std::cout << "\n" << std::endl;
    test["nobuild"] = true;
    icgenerator gen2(test);
    Coords vals(2,202);
    std::cout << "\n" << std::endl;

    for (int i=0;i<4;i++){
        gen2.retIC(vals);
        for (int j=0;j<2;j++){
            std::cout << vals[j] << " ";
        }
    }
    std::cout << "\n" << std::endl;


}

BOOST_AUTO_TEST_CASE(SingleLinTest)
{
	using namespace boost::lambda;
	using namespace std;

	vector<double> x;
	vector<int> y;
    x.push_back(0.);
    x.push_back(0.);
    x.push_back(0.);
    y.push_back(5);
    y.push_back(5);
    y.push_back(5);
    SingleLinIC z(x,1,y);
    Coords vals(3);
    //for (int i=0;i<3;++i)
    //	vals->push_back(std::make_pair(0.,false));

    for (int i=0;i<125;++i){
    	z(vals);
    	for_each(vals.begin(),vals.end(),cout << boost::lambda::_1 << " ");
    	cout << "\n";
    }


}

BOOST_AUTO_TEST_CASE(lineartest)
{
    std::vector<int> xx(2,5);
    test["dims"]  = xx;
    test["dist-type"] = 2;
    
    icgenerator gen(test);
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[25][2]));
    gen.retICs(retArray);

    std::cout << "\n" << std::endl;
    for (int i=0;i<2;i++){
        for (int j=0;j<25;j++){
            std::cout << (*retArray)[j][i] << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n" << std::endl;
    test["nobuild"] = true;
    icgenerator gen2(test);
    Coords vals(x);
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 2; i++)
            vals[i]= 202;
    for (int i=0;i<25;i++){
        gen2.retIC(vals);
        for (int j=0;j<2;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(lineartest2)
{
    std::vector<double> xx(3,0.);
    std::vector<int> yy(3,5);
    test["dims"] = yy;
    test["means"] = xx;
    test["dist-type"] = 2;
    
    std::cout << "\n" << std::endl;
    test["nobuild"] = true;
    icgenerator gen2(test);
    Coords vals(xx);
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 3; i++)
            vals[i]= 202;
    for (int i=0;i<125;i++){
        std::cout <<"crap"<<std::endl;
        gen2.retIC(vals);
        for (int j=0;j<3;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(lineartest3)
{
    std::vector<double> xx(3,0.);
    std::vector<int> yy(3,3);
    test["dims"] = yy;
    test["means"] = xx;
    test["dist-type"] = 2;
    
    std::cout << "\n" << std::endl;
    icgenerator gen2(test);
    Coords vals(xx);
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 3; i++)
            vals[i]= 202;
    for (int i=0;i<27;i++){
        gen2.retIC(vals);
        for (int j=0;j<3;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(lineartest4)
{
    std::vector<double> xx(3,0.);
    std::vector<int> yy(3,3);
    test["dims"] = yy;
    test["means"] = xx;
    test["dist-type"] = 1;

    std::cout << "\n" << std::endl;
    icgenerator gen2(test), gen3(test);
    Coords vals(xx);
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 3; i++)
            vals[i]= 202;
    for (int i=0;i<15;i++){
        gen2.retIC(vals);
        for (int j=0;j<3;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "split here" <<std::endl;
    gen3.seek(15);
    for (int i=15;i<27;i++){
        gen3.retIC(vals);
        for (int j=0;j<3;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(lineartest5)
{
    std::vector<double> xx(3,0.);
    std::vector<int> yy(3,3);
    test["dims"] = yy;
    test["means"] = xx;
    test["dist-type"] = 2;

    std::cout << "\n" << std::endl;
    icgenerator gen2(test), gen3(test);
    Coords vals(xx);
    std::cout << "\n" << std::endl;
    for (int i = 0; i < 3; i++)
            vals[i]= 202;
    for (int i=0;i<15;i++){
        gen2.retIC(vals);
        for (int j=0;j<3;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "split here" <<std::endl;
    gen3.seek(15);
    for (int i=15;i<27;i++){
        gen3.retIC(vals);
        for (int j=0;j<3;j++){
            std::cout << vals[j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n" << std::endl;
}

BOOST_AUTO_TEST_CASE(serialize1)
{
    std::ofstream ofs("test.dat");
    std::vector<double> xx(3,0.);
    std::vector<int> yy(3,5);
    test["dims"] = yy;
    test["means"] = xx;
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


    Coords vals(xx);
    Coords vals2(xx);
    for (int i = 0; i < 3; i++){
            vals[i]= 202;
            vals2[i]= 203;
    }
    for (int i=0;i<125;i++){
        gen.retIC(vals);
        gen2.retIC(vals2);
        for (int j=0;j<3;j++){
            BOOST_CHECK_CLOSE(vals[j],vals2[j],1E-6);
        }
    }

}

BOOST_AUTO_TEST_CASE(serialize2)
{
    std::vector<int> xx(2,5);
    test["dims"]  = xx;
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
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[25][2]));
    boost::shared_ptr<boost::multi_array<double, 2> > retArray2=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[25][2]));
    gen.retICs(retArray);
    gen2.retICs(retArray2);

    std::cout << "\n" << std::endl;
    for (int i=0;i<25;i++){
        for (int j=0;j<2;j++){
            BOOST_CHECK_CLOSE((*retArray)[i][j],(*retArray2)[i][j],1E-6);
        }
    }
}

BOOST_AUTO_TEST_CASE(serialize_member)
{
    std::vector<int> xx(2,5);
    test["dims"]  = xx;
    icgenerator gen(test);

    gen.save("text", "test3.dat");

    icgenerator gen2;

    gen2.load("text", "test3.dat");
    boost::shared_ptr<boost::multi_array<double, 2> > retArray=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[25][2]));
    boost::shared_ptr<boost::multi_array<double, 2> > retArray2=boost::shared_ptr<boost::multi_array<double,2> >(new boost::multi_array<double,2>(boost::extents[25][2]));

    gen2.retICs(retArray2);
    gen.retICs(retArray);

    for (int i=0;i<25;i++){
        for (int j=0;j<2;j++){
            BOOST_CHECK_CLOSE((*retArray)[i][j],(*retArray2)[i][j],1E-6);
        }
    }
}

BOOST_AUTO_TEST_CASE(serialize_wrong)
{
    icgenerator gen(test);

    gen.save("text", "test3.dat");

    icgenerator gen2;

    BOOST_CHECK_THROW(gen2.load("binary", "test3.dat"), std::exception);
}

BOOST_AUTO_TEST_CASE(weights1)
{
    std::vector<double> xx(2,0.);
    std::vector<int> yy(2,5);
    test["dims"] = yy;
    test["means"] = xx;
    //test["nobuild"] = true;`
    anyMap test2;
    test2["weight-func"] = 1;

    icgenerator gen(test,&test2);
    boost::shared_ptr<std::vector<double> > retArray=boost::shared_ptr<std::vector<double> >(new std::vector<double>(25));
    gen.retWeights(retArray);
    std::for_each(retArray->begin(),retArray->end(),std::cout << boost::lambda::_1 << " ");
    std::cout << std::endl;

}

BOOST_AUTO_TEST_CASE(weights2)
{
    std::vector<double> xx(2,0.);
    std::vector<int> yy(2,5);
    test["dims"] = yy;
    test["means"] = xx;
    test["nobuild"] = true;
    anyMap test2;
    test2["weight-func"] = 2;
    test2["ef"] = std::vector<double>(1,0.6666666666);
    test2["ip"] = 1.0;
    test2["sigmas"] = boost::shared_ptr<std::vector<double> >(new std::vector<double>(2,1.));
    test2["means"] = xx;

    icgenerator gen(test,&test2);
    Coords vals(xx);
    for(int i=0;i<25;++i){
       gen.retIC(vals);
       std::cout << gen.retWeight(vals) << " ";
    }
    std::cout << std::endl;

}

BOOST_AUTO_TEST_CASE(longrun)
{
    using namespace timer;
    Timer::create();

    std::vector<double> xx(4,0.);
    std::vector<int> yy(4,50);
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

    icgenerator gen(test,&test2);
    Coords vals(xx);
    TimerStart("main");
    for(int i=0;i<6250000;++i){
       gen.retIC(vals);
       gen.retWeight(vals);
    }
    TimerStop("main");
    TimerReport();
}

BOOST_AUTO_TEST_SUITE_END()

}
