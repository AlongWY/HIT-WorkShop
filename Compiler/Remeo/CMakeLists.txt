cmake_minimum_required(VERSION 3.10)
project(Romeo)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

include(ExternalProject)

## OGDF设置
#ExternalProject_Add(
#        LIB_OGDF
#        PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/external
#        GIT_REPOSITORY https://github.com/ogdf/ogdf.git
#        CMAKE_ARGS -DOGDF_SEPARATE_TESTS=OFF -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DOGDF_WARNING_ERRORS=OFF
#        UPDATE_DISCONNECTED false
#)
#
#ExternalProject_Get_Property(LIB_OGDF install_dir)
#add_library(OGDF STATIC IMPORTED)
#set_property(TARGET OGDF PROPERTY IMPORTED_LOCATION "${install_dir}/lib/libOGDF.a")
#add_library(COIN STATIC IMPORTED)
#set_property(TARGET COIN PROPERTY IMPORTED_LOCATION "${install_dir}/lib/libCOIN.a")
#include_directories("${install_dir}/include")

add_subdirectory(${PROJECT_NAME})
add_subdirectory(${PROJECT_NAME}GUI)
add_subdirectory(${PROJECT_NAME}Lang)
#add_dependencies(${PROJECT_NAME} LIB_OGDF)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Grammer
        ${CMAKE_CURRENT_SOURCE_DIR}/RmoCode
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
