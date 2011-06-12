cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

include_directories (.)
include_directories ("../include/")

include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${NUMPY_INCLUDE_DIR})

find_package(Boost 1.45.0 COMPONENTS program_options date_time serialization filesystem system REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )
include_directories(${MPI_INCLUDE_PATH})

configure_file (
    "${PROJECT_SOURCE_DIR}/config.hpp.in"
    "${PROJECT_BINARY_DIR}/include/config.hpp"
)

link_libraries(${LIBS} -lpthread ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
message("${LIBS}")

set(classical_lib_src input_param.cpp params.cpp pywrapper.cpp pymodule.cpp fields.cpp potential.cpp icgen.cpp zeros.cpp integrator.cpp force.cpp trajs.cpp)
add_library(classical SHARED ${classical_lib_src})

set(EXECUTABLE_OUTPUT_PATH "../")
add_executable (cpp-class main.cpp)
target_link_libraries(cpp-class classical )
if(MPI_FOUND)
    add_executable (cpp-classMPI mainMPI.cpp)
    target_link_libraries(cpp-classMPI classical )
endif(MPI_FOUND)    

GET_TARGET_PROPERTY(LIB_NAME classical LOCATION)
GET_TARGET_PROPERTY(Bar_prefix classical PREFIX)
GET_TARGET_PROPERTY(Bar_suffix classical SUFFIX)
SET(NEW_LIB_NAME ${Bar_prefix}class${Bar_suffix})
 
ADD_CUSTOM_COMMAND(
  TARGET classical
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ../pygui/classical.so
)

