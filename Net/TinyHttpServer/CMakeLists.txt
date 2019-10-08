cmake_minimum_required(VERSION 3.9)
project(httpServer)

set(CMAKE_CXX_STANDARD 11)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS log regex)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(SOURCE_FILES main.cpp HttpServer.cpp HttpServer.h)
    add_executable(httpServer ${SOURCE_FILES})
    target_link_libraries(httpServer pthread ${Boost_LIBRARIES})
endif ()

