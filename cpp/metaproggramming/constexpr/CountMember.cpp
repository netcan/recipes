/*************************************************************************
    > File Name: CountMember.cpp
    > Author: Netcan
    > Descripton: CountMember
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-09 22:23
************************************************************************/
#include <cstdio>

struct AnyType {
    template <typename T> operator T();
};

template <typename T>
consteval size_t CountMember(auto&&... Args) {
    if constexpr (requires { T{ Args... }; }) {
        return CountMember<T>(Args..., AnyType{});
    } else {
        return sizeof...(Args) - 1;
    }
}

struct Test { int a; int b; int c; int d; };
static_assert(CountMember<Test>() == 4);

int main(int argc, char** argv) {
}
