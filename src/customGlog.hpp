// $Id$
/**
 * @file customGlog.hpp
 *
 * custom header which either includes google::glog or creates some macros to mask the fact that its missing
 *
 * @brief custom google::glog header
 *
 * @author Ryan Murray
 * @version 1.00
 */
// $Log$


#ifndef Gflags
    /// macro to print input to custom log file
    #define LOG(X) (std::cout << X << " ")
    /// macro to print input to custom debug log file
    #define DLOG(X) (std::cout << X << " ")
    /// number linking to log type
    #define FATAL   0
    /// number linking to log type
    #define ERROR   1
    /// number linking to log type
    #define WARNING 2
    /// number linking to log type
    #define INFO    3
#else
    #include <glog/logging.h>
#endif
