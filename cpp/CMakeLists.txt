# author by netcan
# @shanghai 2019/07/29 22:16
cmake_minimum_required(VERSION 3.10)
project(Practice LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
link_directories(/usr/lib /usr/local/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})


include_directories(.)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "-std=c99 -Wall")
set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3")
set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-return-type -Wno-unused-result -Wthread-safety -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")

add_subdirectory(oop)
add_subdirectory(clang-plugin)
add_subdirectory(test)
add_subdirectory(lisp-parser)
add_subdirectory(metaproggramming)
add_subdirectory(glLearn)
