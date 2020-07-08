# author by netcan
# @shanghai 2019/09/13 08:42
add_executable(Fibonacci Fibonacci.cpp)
add_executable(ADL ADL.cpp)
add_executable(ConstexprStr ConstexprStr.cpp)
add_executable(StaticRefl StaticRefl.cpp)

add_executable(Relational Relational.cpp)
target_link_libraries(Relational ${GTEST_BOTH_LIBRARIES})
