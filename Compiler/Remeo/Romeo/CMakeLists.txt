cmake_minimum_required(VERSION 3.10)
project(Romeo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SOURCE)
add_library(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME})