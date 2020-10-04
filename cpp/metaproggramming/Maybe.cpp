/*************************************************************************
    > File Name: Maybe.hpp
    > Author: Netcan
    > Descripton: Simple Monad
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-30 22:12
************************************************************************/
#include <type_traits>
#include <variant>
#include <functional>
#include <iostream>

template<typename T>
struct Just {
    T value;
    operator const T&() const { return value; }
    Just(const T& value): value(value) {}
};
struct Nothing {};

template<typename T>
struct Maybe {
    std::variant<Just<T>, Nothing> value;
    Maybe(const T& value): value(value) {}
    Maybe(const Nothing&): value(Nothing{}) {}
};

template<class... Ts>
struct overloaded: Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename T>
auto just(T x) -> Maybe<T> { return x; }

struct AnyType {
    template<typename T> operator T() { return T{}; };
    friend std::ostream& operator<<(std::ostream& os, const AnyType&) { return os; };
};
auto nothing() -> Maybe<AnyType> { return Nothing{}; }

template<typename T, typename F>
auto operator>>=(const Maybe<T>& ma, F&& f) {
    using R = std::invoke_result_t<F, T>;
    return std::visit(overloaded {
        [&](const Just<T>& v) -> R
        { return f(static_cast<T>(v)); },
        [](Nothing) -> R
        { return Nothing{}; }
    }, ma.value);
}

template<typename T>
auto show(const Maybe<T>& value) -> void {
    std::visit(overloaded {
        [](const Just<T>& v)
        { std::cout << "Just " << v << std::endl; },
        [](Nothing)
        { std::cout << "Nothing" << std::endl; }
    }, value.value);
}

int main(int argc, char** argv) {
    auto value = just(3);
    show(value); // Just 3

    show((just(3) >>=
        [](int v) -> Maybe<double> {
            return {v * 1.5};
        }) >>= [](double v) -> Maybe<double> {
            return {v * 1.5};
        }
    ); // Just 6.75

    show((nothing() >>=
        [](int v) -> Maybe<double> {
            return {v * 1.5};
        }) >>= [](double v) -> Maybe<double> {
            return {v * 1.5};
        }
    ); // Nothing

    return 0;
}

