# author by netcan
# @shanghai 2019/09/13 08:42
add_executable(Picture Picture.cpp)
add_executable(LispTest Lisp.cpp)
target_link_libraries(LispTest ${GTEST_BOTH_LIBRARIES})

