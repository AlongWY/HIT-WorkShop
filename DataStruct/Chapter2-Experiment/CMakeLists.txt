cmake_minimum_required(VERSION 3.9)
project(Poly C)

set(CMAKE_C_STANDARD 99)

add_executable(testlist testlist.c list.c)

add_executable(polynomial polymain.c list.c polynomial.c)

target_link_libraries(polynomial m)

