
#include "timer.hpp"

SingleTimer::SingleTimer(std::string name): name_(name), calls_(0), active_(false), wallTime_(0), realTime_(0), wallKids_(0), realKids_(0){}

SingleTimer::SingleTimer(): name_(""), calls_(0), active_(false), wallTime_(0), realTime_(0), wallKids_(0), realKids_(0){}

void SingleTimer::start(){
    ++calls_;
    if (active_) LOG(FATAL) << "Timer " + name_ + " is already active";
    active_=true;
    startCpu_ = std::clock();
    startReal_ = std::time(NULL);
}

std::pair<double,double> SingleTimer::stop(){
    if (!active_) throw std::runtime_error("This timer is not active, it can't be stopped!");
    active_=false;
    std::clock_t endc = std::clock();
    std::time_t endt = std::time(NULL);
    double wout = std::difftime(endc,startCpu_);
    double rout = endt-startReal_;
    wallTime_ += wout;
    realTime_ += rout;
    return std::make_pair(wout,rout);
}

singleTimerVals SingleTimer::report(){
    return boost::make_tuple(name_,calls_,wallTime_,wallKids_,realTime_,realKids_);
}

void SingleTimer::setKids(std::pair<double,double> T){
    (*this).wallKids_ += T.first;
    (*this).realKids_ += T.second;
}

Timer* Timer::m_pInstance = NULL;

Timer* Timer::create(){
    if (!m_pInstance)
        m_pInstance = new Timer;
    return m_pInstance;
}

void Timer::start(std::string name){
    std::pair<hashTable::iterator,bool> ret;

    ret = timerHash_.insert(std::pair<std::string,SingleTimer>(name,SingleTimer(name)));
    ret.first->second.start();
    timerStack_.push(name);
}

void  Timer::stop(std::string name){
    std::pair<double,double> stops;
    if (timerStack_.size() != 0){
        if (timerStack_.top() == name){
            timerStack_.pop();
            stops = timerHash_[name].stop();
        } else {
            LOG(FATAL) << "Timer " + name + " is not at top of the stack, crossing nested timers.";
        }
    } else {
        LOG(FATAL) << "Timer " + name + " is not at top of the stack, crossing nested timers."; 
    }
    if (timerStack_.size() != 0){
        timerHash_[timerStack_.top()].setKids(stops);
    }


}

void Timer::report(){
    if (timerStack_.size() != 0){
        LOG(ERROR) << "Not all timers were stopped, stopping them all now";
        while (!timerStack_.empty()){
            timerHash_[timerStack_.top()].stop();
            timerStack_.pop();
        }
    }
    std::string nname;
    int calls;
    double cputime, cpukids, realtime, realkids;
    LOG(INFO) << std::setfill(' ') << std::setw(15) << std::left << " Timer" << "\t\t\t\t"
        << std::setw(8) << "Calls" << std::setw(26) << std::left << "Total Time (seconds)" 
        << std::setw(20) << "Self Time (seconds)";
    LOG(INFO) << std::setfill(' ') << std::setw(15) << std::left << " -----" << "\t\t\t\t"
        << std::setw(8) << "-----" << std::setw(26) << std::left << "--------------------" 
        << std::setw(20) << "-------------------";
    LOG(INFO) << std::setfill(' ') << std::setw(15) << std::left << "      " << "\t\t\t\t"
        << std::setw(8) << "     " << std::setw(26) << std::left << "   CPU    REAL      " 
        << std::setw(20) << "   CPU    REAL      ";
    BOOST_FOREACH(hashTable::value_type i, timerHash_){
       boost::tie(nname, calls, cputime, cpukids, realtime, realkids) = i.second.report();
        std::ostringstream strs1, strs2, strs3, strs4;
        strs1.precision(2);
        strs1 << cputime/CLOCKS_PER_SEC;
        strs2.precision(2);
        strs2 << (cputime-cpukids)/CLOCKS_PER_SEC;
        strs3.precision(2);
        strs3 << realtime;
        strs4.precision(2);
        strs4 << realtime-realkids;
        std::string str1 = strs1.str();
        std::string str2 = strs2.str();
        std::string str3 = strs3.str();
        std::string str4 = strs4.str();
        LOG(INFO) << std::setfill(' ') << std::setw(15) << std::left << " " + nname << "\t\t\t\t"
            << std::setw(8) << std::right << boost::lexical_cast<std::string>(calls) + "   " 
            << std::setw(26) << std::left << "   " + str1 + "     " + str3
            << std::setw(20) << std::left << "    " + str2 + "     " + str4
            ;
    }
    /*  TODO There is a small problem still with how the CPU and REAL are aligned
        I should alter the last two lines to print out the two columns in line
        Ryan Feb 2, 2011
    */        
}

void TimerStart(std::string name){
	Timer::create()->start(name);
}

void TimerStop(std::string name){
	Timer::create()->stop(name);
}

void TimerReport(){
	Timer::create()->report();
}







