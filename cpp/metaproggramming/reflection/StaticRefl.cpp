/*************************************************************************
    > File Name: StaticRefl.cpp
    > Author: Netcan
    > Descripton: StaticRefl usecase
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-31 20:17
************************************************************************/
#include <cstdio>
#include <iostream>
#include "StaticRefl.hpp"

template<typename T>
void dumpObj(T&& obj, const char* fieldName = "", int depth = 0) {
    auto indent = [depth] {
        for (int i = 0; i < depth; ++i) {
            printf("   ");
        }
    };

    if constexpr(std::is_class_v<std::decay_t<T>>) {
        indent();
        printf("%s%s{\n", fieldName, *fieldName ? ": " : "");
        forEach(obj, [depth](auto&& fieldName, auto&& value) {
            dumpObj(value, fieldName, depth + 1);
        });
        indent();
        printf("}%s\n", depth == 0 ? "" : ",");
    } else {
        indent();
        std::cout << fieldName << ": " << obj << "," << std::endl;
    }
}

DEFINE_STRUCT(Point,
    (double) x,
    (double) y)

DEFINE_STRUCT(Rect,
        (Point) p1,
        (Point) p2,
        (uint32_t) color)

int main(int argc, char** argv) {
    Rect rect {
        {1.2, 3.4},
        {5.6, 7.8},
        12345678,
    };
    dumpObj(rect);

    return 0;
}
