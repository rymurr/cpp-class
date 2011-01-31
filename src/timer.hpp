
#include <time.h>
#include <stack>
#include <string>
#include <iostream>
//#include <boost/timer.cpp>
#include <boost/unordered_set.hpp>

class SingleTimer {

    private:
        std::clock_t startCpu_;
        std::time_t startReal_;
        //clock_t is measured in milliseconds or so, time_t is measured in seconds
        //there is no simple and portable way to measure time with higher resolution
        //live with it
        bool active_;
        std::string name_;
        double calls_;
        double wallTime_;
        double wallKids_;
        double realTime_;
        double realKids_;

    public:
        SingleTimer(std::string);




}



