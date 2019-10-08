cmake_minimum_required(VERSION 3.9)
project(MinTree C)

set(CMAKE_C_STANDARD 99)

add_executable(MinTree MinTree.c Graph.c Graph_tools.c WeightGraph.c Binaryheap.c)

file(COPY "Graph.txt" DESTINATION "./")