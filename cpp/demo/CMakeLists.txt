# author by netcan
# @shanghai 2019/09/13 08:42
add_executable(lispcpp lisp.cpp)
add_executable(Picture picture.cpp)
target_link_libraries(lispcpp ${GTEST_BOTH_LIBRARIES})

add_subdirectory(lispc)
