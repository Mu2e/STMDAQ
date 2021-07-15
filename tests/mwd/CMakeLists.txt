cmake_minimum_required(VERSION 3.15)
project(mwd_cpp)

set(CMAKE_CXX_STANDARD 17)
set(MWD mwd)
include_directories(algo)
add_library(algo STATIC algo/algo.cpp algo/algo.h)

add_executable(${MWD} main.cpp)
target_link_libraries(${MWD} algo)