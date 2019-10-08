cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-Wall -O3 -D__i386__")

add_executable(mountain mountain/mountain.c mountain/clock.c mountain/fcyc2.c)