cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)

add_executable(hello hello.c)

file(COPY "hello.txt" DESTINATION "./")