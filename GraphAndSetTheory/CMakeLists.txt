cmake_minimum_required(VERSION 3.7)
project(Graph)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES Code/main.c Code/Graph.c Code/Graph.h Code/WeightGraph.c Code/WeightGraph.h Code/Graph_tools.c Code/Graph_tools.h Code/Binaryheap.c Code/Binaryheap.h StableMatching/StableMatching.c StableMatching/StableMatching.h StableMatching/stable.c StableMatching/Queue.c StableMatching/Queue.h)
add_executable(Graph ${SOURCE_FILES} StableMatching/stable.c)