#include <ctime>
#include <time.h>
#include <stack>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>
//#include <boost/timer.cpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <glog/logging.h>

typedef boost::tuple<std::string,int,double,double,double,double> singleTimerVals;

class SingleTimer {

    private:
        std::clock_t startCpu_;
        std::time_t startReal_;
        //clock_t is measured in milliseconds or so, time_t is measured in seconds
        //there is no simple and portable way to measure time with higher resolution
        //live with it
        std::string name_;
        int calls_;
        bool active_;
        double wallTime_;
        double realTime_;
        double wallKids_;
        double realKids_;

    public:

        SingleTimer(std::string);

        SingleTimer();

        void start();

        std::pair<double,double> stop();

        singleTimerVals report();

        void setKids(std::pair<double,double>);

};

typedef boost::unordered_map<std::string,SingleTimer> hashTable;

class Timer {

    private:
        Timer(){};
        Timer(Timer const&){};
        Timer& operator=(Timer const&){return (*this);};
        static Timer* m_pInstance;
        std::stack<std::string> timerStack_;
        hashTable timerHash_;

    public:
        void start(std::string);

        void stop(std::string);

        void report();

        static Timer* create();
};

void TimerStart(std::string);
void TimerStop(std::string);
void TimerReport();
