/*************************************************************************
    > File Name: TaskDSLv2.cpp
    > Author: Netcan
    > Descripton: TaskDSLv2
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-27 22:38
************************************************************************/
#include <cstdio>
#include <memory>
#include "TaskDsl.hpp"

int main(int argc, char** argv) {
    __def_task(A, { return Job("TaskA"); });
    __def_task(B, { return Job("TaskB"); });
    __def_task(C, { return Job("TaskC"); });
    __def_task(D, { return Job("TaskD"); });

    __taskbuild(
        __link(A) -> __fork(C, D),
        __merge(C, B) -> A
    );

    return 0;
}
