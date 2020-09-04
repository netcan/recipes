/*************************************************************************
    > File Name: DoubleClick.cpp
    > Author: Netcan
    > Descripton: DoubleClick state machine to demonstrate coroutine
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-04 20:25
************************************************************************/
#include <experimental/coroutine>
#include <optional>
#include <iostream>
#include <cstdio>
#include <cassert>

using namespace std::experimental;
template<typename Signal, typename Result>
struct StateMachine {
    struct promise_type {
        std::optional<Result> res_;
        std::optional<Signal> recent_signal_;
        auto initial_suspend() { return suspend_never{}; }
        auto final_suspend() noexcept { return suspend_always{}; }
        StateMachine get_return_object() {
            return coroutine_handle<promise_type>::from_promise(*this);
        }
        void unhandled_exception() { std::terminate(); }
        void return_value(Result res) { res_ = res; };

        struct SignalAwaiter {
            std::optional<Signal>& recent_signal_;
            SignalAwaiter(std::optional<Signal>& signal):
                recent_signal_(signal) {}

            bool await_ready() { return recent_signal_.has_value(); }
            void await_suspend(coroutine_handle<>) {}
            Signal await_resume() {
                assert(recent_signal_.has_value());
                Signal sig = *recent_signal_;
                recent_signal_.reset();
                return sig;
            }
        };

        struct signal {};
        SignalAwaiter await_transform(signal) {
            return SignalAwaiter{recent_signal_};
        }
    };

    using signal = typename promise_type::signal;

    StateMachine(coroutine_handle<promise_type> coroutine):
        coroutine_(coroutine) {}
    ~StateMachine() { coroutine_.destroy(); }
    bool done() { return coroutine_.done(); }

    void send_signal(Signal signal) {
        coroutine_.promise().recent_signal_ = signal;
        if (! coroutine_.done())
            coroutine_.resume();
    }

    std::optional<Result> get_result() {
        return coroutine_.promise().res_;
    }


private:
    coroutine_handle<promise_type> coroutine_;
};

enum class ButtonPress {
    LEFT_MOUSE,
    RIGHT_MOUSE
};

StateMachine<ButtonPress, std::FILE*> open_file(const char* file_name) {
    using ThisCoroutine = StateMachine<ButtonPress, std::FILE*>;
    ButtonPress first_press = co_await ThisCoroutine::signal{};
    while (true) {
        ButtonPress second_press = co_await ThisCoroutine::signal{};
        if (first_press == second_press &&
                first_press == ButtonPress::LEFT_MOUSE) {
            co_return std::fopen(file_name, "r");
        }
        first_press = second_press;
    }
}

int main(int argc, char** argv) {
    auto machine = open_file("/dev/zero");
    machine.send_signal(ButtonPress::LEFT_MOUSE);
    machine.send_signal(ButtonPress::RIGHT_MOUSE);
    machine.send_signal(ButtonPress::LEFT_MOUSE);
    machine.send_signal(ButtonPress::LEFT_MOUSE);

    std::cout << "machine done: " << machine.done() << std::endl;
    auto result = machine.get_result();
    std::cout << "machine result: " << result.has_value() << std::endl;
    if (result.has_value()) { std::fclose(*result); }

    return 0;
}
