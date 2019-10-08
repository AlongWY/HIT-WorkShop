cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)
add_executable(myint src/myint.c)
add_executable(myspin src/myspin.c)
add_executable(mysplit src/mysplit.c)
add_executable(mystop src/mystop.c)

add_executable(tsh src/tsh.c)

file(COPY "driver/" DESTINATION "./")