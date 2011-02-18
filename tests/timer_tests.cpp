#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TimerTests
#include <boost/test/unit_test.hpp>
#include <boost/tuple/tuple.hpp>
#include <string>
#include <iostream>
#include "timer.hpp"



BOOST_AUTO_TEST_SUITE(TimerTests)


BOOST_AUTO_TEST_CASE(SingleTimerTest1)
{
    std::string name="test1", nname;
    int calls;
    double cputime, cpukids, realtime, realkids;
    SingleTimer x("test1");

    x.start();
    for (int i=0; i<10000000; ++i) int j = i;
    x.stop();
    boost::tie(nname, calls, cputime, cpukids, realtime, realkids) = x.report();
    BOOST_CHECK(nname == name);
    BOOST_CHECK(calls == 1);
    std::cout << cputime/CLOCKS_PER_SEC << " " << cpukids << " " << realtime << " " << realkids << std::endl;

}

BOOST_AUTO_TEST_CASE(TimerTest1)
{
    Timer::create();

    int j=0;
    TimerStart("a");
    TimerStart("d");
    TimerStart("b");
    for (int i=0; i<100000000; ++i) j++;
    TimerStop("b");
    TimerStart("c");
    for (int i=0; i<100000000; ++i) j++;
    TimerStop("c");
    TimerStop("d");
    TimerStart("e");
    for (int i=0; i<100000000; ++i) j++;
    TimerStart("b");
    TimerStop("b");
    TimerStop("e");
    TimerStop("a");
    TimerReport();
    LOG(INFO) << j << std::endl;

}

BOOST_AUTO_TEST_CASE(TimerExcept)
{
    SingleTimer T("d");
    BOOST_CHECK_THROW(T.stop(),timer_except);
}

BOOST_AUTO_TEST_SUITE_END()

