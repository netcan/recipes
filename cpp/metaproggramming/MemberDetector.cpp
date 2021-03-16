/*************************************************************************
    > File Name: MemberDetector.cpp
    > Author: Netcan
    > Descripton: MemberDetector for detect member if exist
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-03-16 22:50
************************************************************************/
#include <iostream>
#include <type_traits>

namespace details {
struct DummyV {
    template<typename T>
    constexpr DummyV& operator=(T&& v) {
        std::cout << "set DummyV is noop!" << std::endl;
        return *this;
    }
};
// scenario: only use lvalue for modify a variable
// Detector:: auto&& v -> decltype(v.field)&
template<typename T, typename Detector,
    typename = decltype(std::declval<Detector>()(std::declval<T>())) >
constexpr auto& MemberWrapper(T& v, Detector detector) { return detector(v); }

// not detect field, noop
constexpr auto MemberWrapper(...) { return DummyV{}; }
}

#define MemWrap(value, field)     \
    details::MemberWrapper(value, \
            [](auto&& v) ->       \
            decltype(v.field)&    \
            { return v.field; })  \

int main(int argc, char** argv) {
    struct Msg {
        int a;
        int b;
    } msg{0, 0};

    MemWrap(msg, a) = 1;
    MemWrap(msg, b) = 2;
    MemWrap(msg, c) = 3; // noop
    std::cout << "msg.a = " << msg.a
        << "; msg.b = " << msg.b << std::endl;

    return 0;
}
