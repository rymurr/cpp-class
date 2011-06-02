cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
#enable_language ( Fortran )

include_directories (.)
include_directories ("../include/")

if (NOT DEFINED Build_MPI)
    set(Build_MPI false)
endif(NOT DEFINED Build_MPI)

INCLUDE (CheckIncludeFiles)
if (${Build_MPI})
   find_package(Boost COMPONENTS mpi)
    if(Boost_FOUND)
        find_package(MPI)
        if(MPI_FOUND)
            include_directories(${MPI_INCLUDE_PATHS})
            set(LIBS ${LIBS} ${MPI_LIBRARIES})
        endif(MPI_FOUND)
     endif(Boost_FOUND)
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)

find_package(Boost 1.45.0 COMPONENTS program_options date_time serialization filesystem system prg_exec_monitor REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

#find_package(GSL)
#if(GSL_FOUND)
#    include_directories(${GSL_INCLUDE_DIR})
#    set(LIBS ${LIBS} ${GSL_LIBRARIES} ) 
#    add_definitions(-DGSL)
#endif(GSL_FOUND)

configure_file (
    "${PROJECT_SOURCE_DIR}/config.hpp.in"
    "${PROJECT_BINARY_DIR}/include/config.hpp"
)

link_libraries(${LIBS} -lpthread ${Boost_LIBRARIES})

set(classical_lib_src input_param.cpp params.cpp pywrapper.cpp pymodule.cpp fields.cpp potential.cpp icgen.cpp zeros.cpp integrator.cpp force.cpp trajs.cpp)
add_library(classical SHARED ${classical_lib_src})

add_executable (cpp-class main.cpp)
target_link_libraries(cpp-class classical )

GET_TARGET_PROPERTY(LIB_NAME classical LOCATION)
GET_TARGET_PROPERTY(Bar_prefix classical PREFIX)
GET_TARGET_PROPERTY(Bar_suffix classical SUFFIX)
SET(NEW_LIB_NAME ${Bar_prefix}class${Bar_suffix})
 
ADD_CUSTOM_COMMAND(
  TARGET classical
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ../pygui/classical.so
)

