cmake_minimum_required(VERSION 3.9)
project(Sort)
set(CMAKE_CXX_STANDARD 11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Charts)

add_executable(Sort sortMain.cpp Sort.h)

# Use the Widgets module from Qt 5.
target_link_libraries(Sort Qt5::Charts)