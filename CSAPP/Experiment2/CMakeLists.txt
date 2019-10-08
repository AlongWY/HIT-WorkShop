cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O -Wall -m32")

add_executable(btest src/btest.c src/bits.c src/decl.c src/tests.c src/btest.h src/bits.h)
add_executable(fshow src/fshow.c)
add_executable(ishow src/ishow.c)
add_executable(rbtest src/bits.c src/btest.c src/decl.c src/tests.c)
