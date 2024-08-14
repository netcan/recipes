/*************************************************************************
    > File Name: DoubleClick.cpp
    > Author: Netcan
    > Descripton: DoubleClick state machine to demonstrate coroutine
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-04 20:25
************************************************************************/
#include <coroutine>
#include <optional>
#include <iostream>
#include <cstdio>
#include <cassert>
#include "CoroutineUtils.hpp"

using namespace std;
template<typename Signal, typename Result>
struct StateMachine: coroutine_base<StateMachine<Signal, Result>> {
    struct promise_type:
        coroutine_base<StateMachine>::
        template promise_base_type<promise_type> {
        std::optional<Result> res_;
        std::optional<Signal> recent_signal_;

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
        co_handle_(coroutine) {}

    void send_signal(Signal signal) {
        co_handle_.promise().recent_signal_ = signal;
        if (! co_handle_.done()) co_handle_.resume();
    }

    std::optional<Result> get_result() {
        return co_handle_.promise().res_;
    }

    coroutine_handle<promise_type> co_handle_;
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

    std::cout << "machine done: " << machine.co_handle_.done() << std::endl;
    auto result = machine.get_result();
    std::cout << "machine result: " << result.has_value() << std::endl;
    if (result.has_value()) { std::fclose(*result); }

    return 0;
}
