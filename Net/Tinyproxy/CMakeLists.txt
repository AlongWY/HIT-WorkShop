cmake_minimum_required(VERSION 3.0)
project(TinyProxy)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON)  # only find release libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(SRC_PATH src)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)              # 多线程

find_package(Boost REQUIRED COMPONENTS regex log date_time)

ExternalProject_Add(toml
        GIT_REPOSITORY https://github.com/skystrife/cpptoml.git
        SOURCE_DIR "${CMAKE_BINARY_DIR}/toml-src"
        BINARY_DIR "${CMAKE_BINARY_DIR}/toml-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND "")

ExternalProject_Get_Property(toml source_dir)
include_directories(${source_dir}/include)

#FILE(GLOB SRC_CODE
#        ${SRC_PATH}/*.c
#        ${SRC_PATH}/*.cpp
#        ${SRC_PATH}/*.hpp)

set(SRC_CODE ${SRC_PATH}/DateTime.cpp
        ${SRC_PATH}/Http.cpp
        ${SRC_PATH}/main.cpp
        )

add_executable(TinyProxy ${SRC_CODE})
add_dependencies(TinyProxy toml)
target_link_libraries(TinyProxy Threads::Threads)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(TinyProxy ${Boost_LIBRARIES})
endif ()

set(CONFIG_FILE config.toml)
FILE(COPY ${CONFIG_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})