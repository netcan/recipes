/*************************************************************************
    > File Name: constexpr_func.cpp
    > Author: Netcan
    > Descripton: constexpr func
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-09 21:43
************************************************************************/
#include <functional>
#include <utility>

int main(int argc, char** argv) {
    // constexpr auto fibonacci(int n);
    auto fibonacci = [](int n) {
        int a = 0, b = 1;
        for (int c = 0; c < n; ++ c) {
            int t = a + b;
            a = b;
            b = t;
        }
        return a;
    };

    constexpr auto v = fibonacci(10);
    static_assert(v == 55);
    return 0;
}
