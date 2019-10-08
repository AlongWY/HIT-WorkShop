cmake_minimum_required(VERSION 3.9)
project(Graph)
set(CMAKE_CXX_STANDARD 11)

add_executable(Graph Graph.cpp GraphMain.cpp)

set(Boost_USE_STATIC_LIBS ON) # only find static libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS)

if (Boost_FOUND)
    ADD_DEFINITIONS(-DUSE_BOOST_LIB)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(Graph ${Boost_LIBRARIES})
endif ()

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(Graph ${CURSES_LIBRARIES})

file(COPY "Graph.txt" DESTINATION "./")