cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
enable_language ( Fortran )

include_directories (.)
include_directories ("../include/")

set(EXECUTABLE_OUTPUT_PATH "../")

IF(APPLE)
   FIND_LIBRARY(ACCELERATE_LIBRARY Accelerate)
   MARK_AS_ADVANCED (ACCELERATE_LIBRARY Accelerate)
   SET(LIBS ${LIBS} ${ACCELERATE_LIBRARY})
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg -p -Wall -arch i386")
        set ( CMAKE_CXX_FLAGS_RELEASE  "-DNDEBUG" )
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg -p -Wall -arch i386")
        set ( CMAKE_CXX_FLAGS_RELEASE  "-DNDEBUG -DBOOST_DISABLE_ASSERTS -O4 -march=core2 -fopenmp -arch i386 -flto" )
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "intel")
        set ( CMAKE_CXX_FLAGS_DEBUG "-C -g -O0  -debug all -traceback -Wall -I/usr/include/x86_64-linux-gnu")
        set ( CMAKE_CXX_FLAGS_RELEASE  "-warn -axP -ipo -O3 -no-prec-div -complex_limited_range -I/usr/include/x86_64-linux-gnu" )
    endif()
ENDIF (APPLE)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg -p -Wall")
        set ( CMAKE_CXX_FLAGS_RELEASE  "-DNDEBUG" )
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg -p -Wall")
        set ( CMAKE_CXX_FLAGS_RELEASE  "-DNDEBUG -DBOOST_DISABLE_ASSERTS -O3 -march=native -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -msse4 -ftree-parallelize-loops=8 -funroll-loops -fopenmp " )
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "intel")
        set ( CMAKE_CXX_FLAGS_DEBUG "-C -g -O0  -debug all -traceback -Wall -I/usr/include/x86_64-linux-gnu")
        set ( CMAKE_CXX_FLAGS_RELEASE  "-warn -axP -ipo -O3 -no-prec-div -complex_limited_range -I/usr/include/x86_64-linux-gnu" )
    endif()
   set(BLA_VENDOR "Intel10_64lp")
   find_package(BLAS REQUIRED)
   include_directories(${BLAS_INCLUDE_DIRS})
   set(LIBS ${LIBS} ${BLAS_LIBRARIES} ) 
ENDIF ("${CMAKE_SYSTEM}" MATCHES "Linux")

SET(Boost_USE_MULTITHREADED "OFF")
SET(Boost_ADDITIONAL_VERSIONS "0.99" "0.99.0" "1.78" "1.78.0" "1.41.0" "1.40.0" "1.42.0" "1.36.0" "1.45.0" "1.46.0" "1.46.1")

find_package(Boost COMPONENTS python)
if(Boost_FOUND)
    find_package(PythonLibs 2.6) 
    if(PYTHONLIBS_FOUND)
        include_directories(${PYTHON_INCLUDE_DIRS})
        set(LIBS ${LIBS} ${PYTHON_LIBRARIES})
        add_definitions(-DPYTHON)
    endif(PYTHONLIBS_FOUND)
endif(Boost_FOUND)

find_package(Boost 1.45.0 COMPONENTS program_options date_time serialization filesystem system REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

#find_package(GSL)
#if(GSL_FOUND)
#    include_directories(${GSL_INCLUDE_DIR})
#    set(LIBS ${LIBS} ${GSL_LIBRARIES} ) 
#    add_definitions(-DGSL)
#endif(GSL_FOUND)

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

