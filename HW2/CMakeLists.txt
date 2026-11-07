cmake_minimum_required(VERSION 3.20)

project(HW2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hw2
    src/main.cpp
    src/dense_matrix.cpp
)

target_include_directories(hw2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)
add_executable(test_matrix
    tests/test_matrix.cpp
    src/dense_matrix.cpp
)

target_include_directories(test_matrix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)
if(MSVC)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG}")

    target_compile_options(test_matrix PRIVATE
        "$<$<CONFIG:Debug>:/fsanitize=address>"
    )

    target_link_options(test_matrix PRIVATE
        "$<$<CONFIG:Debug>:/INCREMENTAL:NO>"
    )
endif()

add_executable(bench_matrix
    bench/bench_matrix.cpp
    src/dense_matrix.cpp
)

target_include_directories(bench_matrix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)