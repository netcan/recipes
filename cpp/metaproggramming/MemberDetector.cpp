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
// Detector:: auto&&'a v -> decltype(v.field)&'a
template<typename T, typename Detector,
    typename = decltype(std::declval<Detector>()(std::declval<T>()))>
constexpr decltype(auto) MemberWrapper(T& v, Detector detector)
{ return detector(v); }

// not detect field, noop
constexpr auto MemberWrapper(...) { return DummyV{}; }

///////////////////////////////////////////////////////////////////////////////
// If a member isn't exist, invoke `then'; exist invoke `alternate'
template<typename T, typename Then, typename Alternate>
struct MemberExistIfBase {
    constexpr MemberExistIfBase(
            T& value,
            Then then,
            Alternate alternate):
        value(value),
        then(std::move(then)),
        alternate(std::move(alternate)) {};

    T& value;
    Then then;
    Alternate alternate;
};

template<typename T,
    typename Then, typename Alternate,
    typename = void>
struct MemberExistIf
    : private MemberExistIfBase<T, Then, Alternate> {
    using Base = MemberExistIfBase<T, Then, Alternate>;
    using Base::Base;
    constexpr void operator()() { Base::alternate(Base::value); }
};

template<typename T,
    typename Then, typename Alternate>
struct MemberExistIf<T, Then, Alternate,
    std::void_t<decltype(std::declval<Then>()(std::declval<T>()))>>
    : private MemberExistIfBase<T, Then, Alternate> {
    using Base = MemberExistIfBase<T, Then, Alternate>;
    using Base::Base;
    constexpr void operator()() { Base::then(Base::value); }
};

// C++17 deduction guide
template<typename T,
    typename Then, typename Alternate>
MemberExistIf(T, Then, Alternate)
    -> MemberExistIf<T, Then, Alternate>;
}

#define MemWrap(_value, _field)        \
    details::MemberWrapper(_value,     \
            [](auto&& _value) ->       \
            decltype(_value._field)&   \
            { return _value._field; })

#define MemIf(_value, _field, _then, _alternate)       \
    details::MemberExistIf(_value,                     \
        [](auto&& _value) -> decltype(_value._field) { \
            _then                                      \
            return _value._field;                      \
        },                                             \
        [](auto&& _value){                             \
            _alternate                                 \
        })()

#define MemThen(...) __VA_ARGS__
#define MemElse(...) __VA_ARGS__

int main(int argc, char** argv) {
    struct Msg {
        int a;
        int b;
        struct {
            struct {
                int c[10];
            } d[10];
        } nest;
    } msg{0, 0};

    MemWrap(msg, a) = 1;
    MemWrap(msg, b) = 2;
    MemWrap(msg, c) = 3; // noop

            msg .nest.d[0].c[0]  = 4;
    MemWrap(msg, nest.d[0].c[0]) = 4;
    MemWrap(msg, nest.d[0].e) = 4; // noop

    std::cout << "msg.a = " << msg.a
        << "; msg.b = " << msg.b
        << "; msg.nest = " << msg.nest.d[0].c[0]
        << std::endl;

    MemIf(msg, c,
        MemThen(
            msg.c += 10;
            puts("msg.c in msg!");
        ),
        MemElse(
            puts("msg.c not in msg!");
        )
    );

    MemIf(msg, a,
        MemThen(
            msg.a += 10;
            puts("msg.a in msg!");
        ),
        MemElse(
            puts("msg.a not in msg!");
        )
    );

    std::cout << "msg.a = " << msg.a
        << "; msg.b = " << msg.b
        << "; msg.nest = " << msg.nest.d[0].c[0]
        << std::endl;

    return 0;
}
