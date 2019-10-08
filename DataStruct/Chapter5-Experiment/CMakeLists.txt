cmake_minimum_required(VERSION 3.9)
project(Avl)
set(CMAKE_CXX_STANDARD 11)

add_executable(AVL AvlMain.cpp AvlTree.h)

file(COPY "Data.txt" DESTINATION "./")