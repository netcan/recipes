/*************************************************************************
    > File Name: StaticRefl.cpp
    > Author: Netcan
    > Descripton: StaticRefl usecase
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-31 20:17
************************************************************************/
#include <cstdio>
#include <iostream>
#include <sstream>
#include "StaticRefl.hpp"

template<typename T>
void serializeObj(std::ostream& out, T&& obj,
        const char* fieldName = "", int depth = 0) {
    auto indent = [&] {
        for (int i = 0; i < depth; ++i)
        { out << "    "; }
    };

    if constexpr(IsReflected_v<std::decay_t<T>>) {
        indent();
        out << fieldName << (*fieldName ? ": {" : "{") << std::endl;
        forEach(std::forward<T>(obj),
                [&](auto&& fieldName, auto&& value)
                { serializeObj(out, value, fieldName, depth + 1); });
        indent();
        out << "}" << std::endl;
    } else {
        indent();
        out << fieldName << ": " << obj << std::endl;
    }
}

template<typename T>
void deserializeObj(std::istream& in, T&& obj,
        const char* fieldName = "") {
    if constexpr(IsReflected_v<std::decay_t<T>>) {
        std::string token;
        in >> token; // eat '{'
        if (*fieldName) {
            in >> token; // WARNING: needs check fieldName valid
        }

        forEach(std::forward<T>(obj),
                [&](auto&& fieldName, auto&& value)
                { deserializeObj(in, value, fieldName); });

        in >> token; // eat '}'
    } else {
        if (*fieldName) {
            std::string token;
            in >> token; // WARNING: needs check fieldName valid
        }
        in >> obj; // dump value
    }
}

DEFINE_STRUCT(Point,
    (double) x,
    (double) y);

DEFINE_STRUCT(Rect,
    (Point) p1,
    (Point) p2,
    (uint32_t) color);

int main(int argc, char** argv) {
    std::stringstream result; // serialize result
    {
        Rect rect {
            {1.2, 3.4},
            {5.6, 7.8},
            12345678,
        };
        serializeObj(result, rect);
    }
    std::cout << "serialize rect result:" << std::endl
        << result.str() << std::endl;

    Rect rect2;
    deserializeObj(result, rect2);
    std::cout << "deserialize rect result:" << std::endl;
    serializeObj(std::cout, rect2);

    return 0;
}
