cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O -Wall -m32")

add_executable(hellowin src/hellowin.c)
add_executable(hellolinux src/hellolinux.c)
add_executable(datatype src/datatype.c)
add_executable(showbytes src/showbytes.c)
add_executable(sysinfo src/sysinfo.c)
add_executable(sum src/sum.c)
add_executable(float src/float.c)