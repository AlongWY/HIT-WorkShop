cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O -Wall -m32")

add_executable(Sample src/Sample.c)