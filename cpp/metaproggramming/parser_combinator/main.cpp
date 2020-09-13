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
#include <variant>
using namespace ParserCombinator;

constexpr auto eat_whitespace() {
    return many(oneOf(" \t\r\n"),
            std::monostate{},
            [](auto m, auto) { return m; });
};

constexpr auto constant() {
    constexpr auto digit = oneOf("0123456789");
    return atLeast(digit, 0, [](int acc, char c) {
        return acc * 10 + (c - '0');
    });
};

constexpr auto expr() {
    constexpr auto op = eat_whitespace() < oneOf("+*") > eat_whitespace();
    return bind(op, [](char o, ParserInput result) -> ParserResult<int> {
        if (o == '+') {
            return separatedBy(constant(), eat_whitespace(), 0, std::plus<int>{})(result);
        }
    });
};

constexpr auto lisp() {
    return [](ParserInput s) {
        auto p = makeCharParser('(') < expr() > makeCharParser(')');
        return (eat_whitespace() < p > eat_whitespace())(s);
    };
};

int main(int argc, char** argv) {
    const char* text = R"(
    (+ 123 456)
    )";
    auto r = lisp()(text);
    if (r) {
        std::cout << "(" << r->first << ", " << r->second << ")";
    }

    return 0;
}
