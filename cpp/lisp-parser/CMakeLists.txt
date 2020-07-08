# author by netcan
# @shanghai 2019/09/13 17:37

add_library(lispc SHARED lisp.c)
target_link_libraries(lispc edit mpc)

add_executable(lispc-bin main.c)
target_link_libraries(lispc-bin lispc)

add_executable(lispc-test test.cpp)
target_link_libraries(lispc-test lispc ${GTEST_BOTH_LIBRARIES})
