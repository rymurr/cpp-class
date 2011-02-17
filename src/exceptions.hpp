
#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <boost/exception/all.hpp>
#include <string>

namespace classical{

    typedef boost::error_info<struct info_string,std::string> err_info;
    typedef boost::error_info<struct dims,std::pair<int,int> > dims_info;
    struct exception_base: virtual std::exception, virtual boost::exception { };
    struct out_of_trajectories: virtual exception_base { };
    struct archive_error: virtual exception_base { };
    struct icgen_input_error: virtual exception_base { };
    struct invalid_parameter: virtual exception_base { };
    struct invalid_input: virtual exception_base { };



}
#endif
