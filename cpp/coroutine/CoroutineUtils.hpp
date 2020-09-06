/*************************************************************************
    > File Name: CoroutineUtils.hpp
    > Author: Netcan
    > Descripton: CoroutineUtils
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-06 19:43
************************************************************************/
#pragma once
#include <experimental/coroutine>

template<typename Future>
class coroutine_base {
    Future& get_future() { return static_cast<Future&>(*this); }
protected:
    using suspend_never = std::experimental::suspend_never;
    using suspend_always = std::experimental::suspend_always;
    template<typename U>
    using coroutine_handle = std::experimental::coroutine_handle<U>;

    template<typename Promise>
    struct promise_base_type {
        auto initial_suspend() { return suspend_never{}; }
        auto final_suspend() noexcept { return suspend_always{}; }
        Future get_return_object() {
            return coroutine_handle<Promise>::from_promise(static_cast<Promise&>(*this));
        }
        void unhandled_exception() { std::terminate(); }
    };

    void resume() { if (! get_future().co_handle_.done()) get_future().co_handle_.resume(); }

public:
    ~coroutine_base() { get_future().co_handle_.destroy(); }

};
