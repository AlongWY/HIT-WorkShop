cmake_minimum_required(VERSION 3.10)
project(RomeoLang LANGUAGES CXX)

aux_source_directory(src SOURCE)
add_library(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} Romeo)

add_executable(${PROJECT_NAME}Dev dev.cpp)
target_link_libraries(${PROJECT_NAME}Dev ${PROJECT_NAME})

