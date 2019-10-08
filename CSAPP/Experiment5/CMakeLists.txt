cmake_minimum_required(VERSION 3.8)
project(CSCPP C)

set(CMAKE_C_STANDARD 99)
add_executable(csim src/csim.c src/cachelab.c src/cachelab.h)
add_executable(test-trans src/test-trans.c src/trans.c src/cachelab.c)
add_executable(tracegen src/tracegen.c src/trans.c src/cachelab.c)

file(COPY "src/traces" DESTINATION "./")
file(COPY "src/test-csim" "src/csim-ref" DESTINATION "./")
file(COPY "src/driver.py" DESTINATION "./")