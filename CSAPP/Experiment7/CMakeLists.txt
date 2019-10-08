cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-Wall -O2 -m32")

set(IMPORT_FILES src/mdriver.c src/fsecs.c src/memlib.c src/clock.c src/fcyc.c src/ftimer.c src/config.h)
add_executable(mdriver src/mm.c ${IMPORT_FILES})
add_executable(mdriver-im src/mm-implicit.c ${IMPORT_FILES})

file(COPY "src/traces" DESTINATION "./")