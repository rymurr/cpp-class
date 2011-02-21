
#ifndef Gflags
    #define LOG(X) (std::cout << X << " ")
    #define DLOG(X) (std::cout << X << " ")
    #define FATAL   0
    #define ERROR   1
    #define WARNING 2
    #define INFO    3
#else
    #include <glog/logging.h>
#endif
