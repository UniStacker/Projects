cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TESTS "Build the tests" ON)

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
