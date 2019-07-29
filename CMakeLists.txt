# author by netcan
# @shanghai 2019/07/29 22:16
cmake_minimum_required(VERSION 3.10)
project(CodeSnippet LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(main main.cpp)
target_link_libraries(main ${GTEST_BOTH_LIBRARIES})
set_property(TARGET main PROPERTY CXX_STANDARD 17)
set_property(TARGET main PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET main PROPERTY CXX_EXTENSIONS OFF)
include_directories(main PRIVATE template)
target_compile_options(main PRIVATE
    -Wall
    -Wno-sign-compare
    -Wno-return-type
    -Wno-unused-result
    -Wthread-safety
    )
