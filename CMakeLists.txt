set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
project(cpp-nlevel)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXY_CFG "doc/cpp-class.cfg")
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${DOXY_CFG})
endif()

add_subdirectory(src)
add_subdirectory(tests)
enable_testing()
