/*************************************************************************
    > File Name: Task.cpp
    > Author: Netcan
    > Descripton: Task coroutine
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-06 19:32
************************************************************************/
#include <iostream>
#include "CoroutineUtils.hpp"

using namespace std;
struct task: coroutine_base<task> {
    struct promise_type: promise_base_type<promise_type> {
        struct final_awaiter {
            bool await_ready() noexcept { return false; }
            void await_suspend(coroutine_handle<promise_type> h) noexcept {
                if (h.promise().continuation) {
                    h.promise().continuation.resume();
                }
            }
            void await_resume() noexcept {}
        };
        auto final_suspend() noexcept { return final_awaiter{}; }
        auto initial_suspend() { return suspend_always{}; }
        void return_void() {}
        coroutine_handle<> continuation;
    };
    task(coroutine_handle<promise_type> coroutine):
        co_handle_(coroutine) {}
    using coroutine_base::resume;

///////////////////////////////////////////////////////////////////////////////
    struct awaiter {
        bool await_ready() { return false; }
        void await_suspend(coroutine_handle<promise_type> continuation) {
            co_handle_.promise().continuation = continuation;
            co_handle_.resume();
        }
        void await_resume() {}
        awaiter(coroutine_handle<promise_type> co_handle): co_handle_(co_handle) {}
        coroutine_handle<promise_type> co_handle_;
    };

    awaiter operator co_await() { return {co_handle_}; };
    coroutine_handle<promise_type> co_handle_;
};

task completes_synchronously() {
    co_return;
}

task loop_synchronously(size_t count) {
    std::cout << "enter loop_synchronously" << std::endl;
    for (size_t i = 0; i < count; ++i)
        co_await completes_synchronously(); // stack overflow!
    std::cout << "end loop_synchronously" << std::endl;
}

int main(int argc, char** argv) {
    loop_synchronously(1000000).resume();

    return 0;
}
