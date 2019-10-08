cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(myls myls.c csapp.c)
add_executable(waitpid waitpid.c csapp.c)

target_link_libraries(waitpid Threads::Threads)