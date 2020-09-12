# author by netcan
# @shanghai 2019/09/13 08:42
add_executable(Fibonacci Fibonacci.cpp)
add_executable(ADL ADL.cpp)
add_executable(TaskDSL TaskDSL.cpp)
add_executable(TaskDSLv2 task_dsl_v2/main.cpp)
add_executable(ConstexprStr ConstexprStr.cpp)
add_executable(StaticRefl reflection/StaticRefl.cpp)
add_executable(StaticRefl_TupleImpl reflection/StaticRefl_TupleImpl.cpp)
add_executable(ParserCombinator parser_combinator/main.cpp)
add_executable(PartiallyOrdering PartiallyOrdering)
add_executable(Datatable Datatable.cpp)
add_executable(TypelistTest TypelistTest.cpp)
add_executable(Relational Relational.cpp)

include_directories(.)
target_link_libraries(Relational ${GTEST_BOTH_LIBRARIES})
target_link_libraries(Datatable ${GTEST_BOTH_LIBRARIES})
