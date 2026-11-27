cmake_minimum_required(VERSION 3.20)

project(HW5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

add_executable(server
    src/server.cpp
    src/http.cpp
)

add_executable(client
    src/client.cpp
    src/http.cpp
)

add_executable(test_http 
    tests/test_http.cpp
    src/http.cpp
)

foreach(hw5_target IN ITEMS server client test_http)
    target_include_directories(${hw5_target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    )

    target_link_libraries(${hw5_target} PRIVATE Ws2_32)

    if(MSVC)
        target_compile_options(${hw5_target} PRIVATE
            /utf-8
            "$<$<CONFIG:Release>:/O2>"
        )
    endif()
endforeach()

if(MSVC)
    target_compile_options(test_http PRIVATE
        "$<$<CONFIG:Debug>:/fsanitize=address>"
    )

    target_link_options(test_http PRIVATE
        "$<$<CONFIG:Debug>:/INCREMENTAL:NO>"
    )
endif()

enable_testing()
add_test(NAME http COMMAND test_http)
