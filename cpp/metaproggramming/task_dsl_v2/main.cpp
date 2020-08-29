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
    __def_task(E, { return Job("TaskE"); });
    __def_task(F, { return Job("TaskF"); });

    __taskbuild(
        __chain(__job(A) -> __job(B) -> __job(C)),
        __chain(__fork(B, E) -> __merge(F, E) -> __job(A)),
        __chain(__some_job(B, E) -> __job(C))
    );

    return 0;
}
