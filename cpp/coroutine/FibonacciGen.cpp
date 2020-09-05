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

using namespace std::experimental;

struct FiboFuture {
    struct promise_type {
        int value_;
        auto initial_suspend() { return suspend_never{}; }
        auto final_suspend() noexcept { return suspend_always{}; }
        FiboFuture get_return_object()
        { return {coroutine_handle<promise_type>::from_promise(*this)}; }
        void unhandled_exception() { std::terminate(); }

        auto yield_value(int value) {
            value_ = value;
            return suspend_always{};
        }
        void return_void() {}
    };

    FiboFuture(coroutine_handle<promise_type> handle): handle_(handle) {}

    operator int() { return handle_.promise().value_; }
    void resume() { if (! handle_.done()) handle_.resume(); }
    ~FiboFuture() { handle_.destroy(); }

private:
    coroutine_handle<promise_type> handle_;
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
