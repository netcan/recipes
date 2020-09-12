/*************************************************************************
    > File Name: main.cpp
    > Author: Netcan
    > Descripton: ParserCombinator
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-12 19:56
************************************************************************/
#include "ParserCombinator.hpp"
#include <iostream>
using namespace ParserCombinator;

int main(int argc, char** argv) {
    const char* text = "123568";
    auto p = many(oneOf("012345678"), 0, [](int acc, char c) -> int {
        return acc * 10 + (c - '0');
    });
    auto r = p(text);
    if (r.has_value()) {
        std::cout << "(" << r->first << ", " << r->second << ")" << std::endl;
    }

    return 0;
}
