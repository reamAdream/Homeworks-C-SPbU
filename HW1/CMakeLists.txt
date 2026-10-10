cmake_minimum_required(VERSION 3.10)
project("HW 1" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(hw1 src/main.cpp)
add_executable(test_sort tests/test_sort.cpp)
add_executable(bench_sort bench/bench_sort.cpp)