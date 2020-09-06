/*************************************************************************
    > File Name: Fibonacci.cpp
    > Author: Netcan
    > Descripton: Fibonacci generator
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-05 20:39
************************************************************************/
#include <experimental/coroutine>
#include <tuple>
#include <iostream>
#include "CoroutineUtils.hpp"

using namespace std::experimental;

struct FiboFuture: coroutine_base<FiboFuture> {
    struct promise_type: promise_base_type<promise_type> {
        int value_;
        auto yield_value(int value) {
            value_ = value;
            return suspend_always{};
        }
        void return_void() {}
    };

    FiboFuture(coroutine_handle<promise_type> handle): co_handle_(handle) {}
    using coroutine_base::resume;

    operator int() { return co_handle_.promise().value_; }

    coroutine_handle<promise_type> co_handle_{};
};

FiboFuture generate_fibo() {
    int i = 0, j = 1;
    while (true) {
        co_yield j;
        std::tie(i, j) = std::make_pair(j, i + j);
    }
}

int main(int argc, char** argv) {
    for (auto x = generate_fibo(); x < 1000; x.resume()) {
        std::cout << "fibo: " << x << std::endl;
    }

    return 0;
}
