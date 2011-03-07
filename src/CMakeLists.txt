cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
enable_language ( Fortran )

include_directories (.)

set(EXECUTABLE_OUTPUT_PATH "../")

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg -p -Wall -arch i386")
    set ( CMAKE_CXX_FLAGS_RELEASE  "" )
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -arch i386 -g3 -pg -p -Wall")
    set ( CMAKE_CXX_FLAGS_RELEASE  "" )
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "intel")
    set ( CMAKE_CXX_FLAGS_DEBUG "-C -g -O0  -debug all -traceback -Wall")
    set ( CMAKE_CXX_FLAGS_RELEASE  "-warn -axP -ipo -O3 -no-prec-div -complex_limited_range" )
#    set ( CMAKE_CXX_FLAGS_PARALLEL "-DPARALLEL -DBOOST_DISABLE_ASSERTS -axSSE4.2 -ipo -O3 -no-prec-div -complex_limited_range -openmp " )
endif()
 


IF(APPLE)
   FIND_LIBRARY(ACCELERATE_LIBRARY Accelerate)
   MARK_AS_ADVANCED (ACCELERATE_LIBRARY Accelerate)
   SET(LIBS ${LIBS} ${ACCELERATE_LIBRARY})
ENDIF (APPLE)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
   set(BLA_VENDOR "Intel10_64lp")
   find_package(BLAS REQUIRED)
   include_directories(${BLAS_INCLUDE_DIRS})
   set(LIBS ${LIBS} ${BLAS_LIBRARIES} ) 
ENDIF ("${CMAKE_SYSTEM}" MATCHES "Linux")

SET(Boost_USE_MULTITHREADED "OFF")
SET(Boost_ADDITIONAL_VERSIONS "0.99" "0.99.0" "1.78" "1.78.0" "1.41.0" "1.40.0" "1.42.0" "1.36.0" "1.45.0")

find_package(Boost 1.45.0 COMPONENTS python)
if(Boost_FOUND)
    find_package(PythonLibs 2.7) 
    if(PYTHONLIBS_FOUND)
        include_directories(${PYTHON_INCLUDE_DIRS})
        set(LIBS ${LIBS} ${PYTHON_LIBRARIES})
        add_definitions(-DPYTHON)
    endif(PYTHONLIBS_FOUND)
endif(Boost_FOUND)

find_package(Boost 1.45.0 COMPONENTS program_options date_time serialization filesystem system REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

find_package(GSL)
if(GSL_FOUND)
    include_directories(${GSL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${GSL_LIBRARIES} ) 
    add_definitions(-DGSL)
endif(GSL_FOUND)

find_package(GFlags)
if(GFlags_FOUND)
    find_package(Glog)
    if(Glog_FOUND)
        include_directories(${GFlags_INCLUDE_DIRS})
        include_directories(${Glog_INCLUDE_DIRS})
        link_directories(${GFlags_LIBRARY_DIRS})
        set(LIBS ${LIBS} ${GFlags_LIBS})
        set(LIBS ${LIBS} ${Glog_LIBRARIES} -lglog)
        add_definitions(-DGflags)
    endif(Glog_FOUND)
endif(GFlags_FOUND)

find_package(GooglePerfTools)
if(GOOGLE_PERFTOOLS_FOUND)
    include_directories(${GOOGLE_PERFTOOLS_INCLUDE_DIR})
    set(LIBS ${LIBS} ${TCMALLOC_LIBRARIES} ${STACKTRACE_LIBRARIES} ${PROFILER_LIBRARIES})
    add_definitions(-DGooglePerfTools)
endif(GOOGLE_PERFTOOLS_FOUND)

link_libraries(${LIBS} -lpthread ${Boost_LIBRARIES})

set(classical_lib_src input_param.cpp params.cpp input_funcs.cpp pywrapper.cpp pymodule.cpp fields.cpp potential.cpp icgen.cpp)
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

