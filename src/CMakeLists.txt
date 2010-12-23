cmake_minimum_required( VERSION 2.6 FATAL_ERROR )
enable_language ( Fortran )

include_directories (.)

set(EXECUTABLE_OUTPUT_PATH "../")

if (CMAKE_COMPILER_IS_GNUCXX)
    set ( CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -pg -p --enable-fully-dynamic-string")
    set ( CMAKE_CXX_FLAGS_RELEASE  "" )
else ()
    set ( CMAKE_CXX_FLAGS_DEBUG "-C -g -O0  -debug all -traceback")
    set ( CMAKE_CXX_FLAGS_RELEASE  "-warn -axP -ipo -O3 -no-prec-div -complex_limited_range" )
    set ( CMAKE_CXX_FLAGS_PARALLEL "-DPARALLEL -DBOOST_DISABLE_ASSERTS -axSSE4.2 -ipo -O3 -no-prec-div -complex_limited_range -openmp " )
endif ()

#FIND_PACKAGE(VTK REQUIRED)
##link_directories ( ${VTK_LIBRARY_DIRS} )
##include_directories ( ${VTK_INCLUDE_DIRS} )
#INCLUDE(${VTK_USE_FILE})


IF(APPLE)
   #SET(GUI_TYPE MACOSX_BUNDLE)
   #INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
   #FIND_LIBRARY(CARBON_LIBRARY Carbon)
   #FIND_LIBRARY(QUICKTIME_LIBRARY QuickTime )
   #FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
   FIND_LIBRARY(ACCELERATE_LIBRARY Accelerate)
   MARK_AS_ADVANCED (ACCELERATE_LIBRART Accelerate)
   SET(EXTRA_LIBS ${ACCELERATE_LIBRARY})
ENDIF (APPLE)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
   set(BLA_VENDOR "Intel10_64lp")
   find_package(BLAS REQUIRED)
   include_directories(${BLAS_INCLUDE_DIRS})
   set(LIBS ${LIBS} ${BLAS_LIBRARIES} ) 
ENDIF ("${CMAKE_SYSTEM}" MATCHES "Linux")

SET(Boost_USE_MULTITHREADED "OFF")
SET(Boost_ADDITIONAL_VERSIONS "0.99" "0.99.0" "1.78" "1.78.0" "1.41.0" "1.40.0" "1.42.0" "1.36.0" "1.45.0")
find_package(Boost 1.45.0 COMPONENTS program_options date_time python REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES("/usr/include/python2.6")
LINK_DIRECTORIES("/usr/lib/python2.6")

find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIR})
set(LIBS ${LIBS} ${GSL_LIBRARIES} ) 

find_package(FFTW REQUIRED)
include_directories(${FFTW_INCLUDE_DIR})
set (LIBS ${LIBS} ${FFTW_LIBRARIES} ${FFTW_LIBRARIES_THREADED})

link_libraries(${LIBS} -lpthread -lpython2.6 ${Boost_LIBRARIES} -lglog -lgflags)


add_definitions(-DCHUNKSIZE=10)
add_definitions(-DTESTING)

set(classical_lib_src input_param.cpp params.cpp input_funcs.cpp pywrapper.cpp pymodule.cpp fields.cpp)
add_library(classical SHARED ${classical_lib_src}  )

add_executable (cpp-class main.cpp)
#add_executable (cpp-class main.cpp ${classical_lib_src})
#target_link_libraries(cpp-molion vtkHybrid vtkWidgets ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(cpp-class classical )
#target_link_libraries(cpp-class ${Boost_LIBRARIES})

GET_TARGET_PROPERTY(LIB_NAME classical LOCATION)
GET_TARGET_PROPERTY(Bar_prefix classical PREFIX)
GET_TARGET_PROPERTY(Bar_suffix classical SUFFIX)
SET(NEW_LIB_NAME ${Bar_prefix}class${Bar_suffix})
 
ADD_CUSTOM_COMMAND(
  TARGET classical
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ../classical.so
)

