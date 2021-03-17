/*************************************************************************
    > File Name: MemberDetector.cpp
    > Author: Netcan
    > Descripton: MemberDetector for detect member if exist
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-03-16 22:50
************************************************************************/
#include <iostream>
#include <memory>
#include <type_traits>

namespace details {
struct DummyV {
    template<typename T>
    constexpr DummyV& operator=(T&& v) {
        std::cout << "set DummyV is noop!" << std::endl;
        return *this;
    }
};

struct NoField {};
struct ExistField: NoField {};

template<typename T>
constexpr bool IsDeref = !std::is_pointer_v<T>;

template<typename... Ts>
constexpr bool IsDeref<std::shared_ptr<Ts...>> = false;

template<typename... Ts>
constexpr bool IsDeref<std::unique_ptr<Ts...>> = false;

// scenario: only use lvalue for read/modify a variable
// when read a not exist field, return default value
// Detector:: auto&&'a v -> decltype(v.field)&'a
template<typename T, typename Detector,
    typename Ret = std::decay_t<decltype(std::declval<Detector>()(std::declval<T>()))>,
    typename D = Ret>
constexpr decltype(auto) MemberWrapper(T& v, Detector detector, ExistField, D&& = {}) {
    static_assert(IsDeref<T>, "value must be deref!");
    return detector(v);
}

// not detect field, noop

template<typename T, typename Detector, typename D = DummyV>
constexpr decltype(auto) MemberWrapper(T&, Detector, NoField, D&& defaut = {}) {
    static_assert(IsDeref<T>, "value must be deref!");
    return std::forward<D>(defaut);
}


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

// MemWrap(_value, _field, [default value])
#define MemWrap(_value, _field, ...)  \
    details::MemberWrapper(_value,    \
            [](auto&& value) ->       \
            decltype(value._field)&   \
            { return value._field; }, \
            details::ExistField{},    \
            ##__VA_ARGS__)


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
    };

    // write a field
    {
        Msg msg{};
        MemWrap(msg, a) = 1;
        MemWrap(msg, b) = 2;
        MemWrap(msg, c) = 3; // noop

                msg .nest.d[0].c[0]  = 4;
        MemWrap(msg, nest.d[0].c[0]) = 4;
        MemWrap(msg, nest.d[0].e) = 4; // noop

        printf("msg.a = %d msg.b = %d msg.nest = %d\n",
                msg.a, msg.b, msg.nest.d[0].c[0]);
    }

    // read a field
    {
        Msg msg{};
        MemWrap(msg, a) = 1;
        MemWrap(msg, b) = 2;
        MemWrap(msg, nest.d[0].c[0]) = 4;

        printf("msg.a = %d msg.b = %d "
               "msg.nest = %d msg.noexist = %d\n",
                MemWrap(msg, a, -1),
                MemWrap(msg, b, -1),
                MemWrap(msg, nest.d[0].c[0], -1),
                MemWrap(msg, noexist, -1));

    }

    // static check
    {
        auto msg = std::make_unique<Msg>();
        // error: static assertion failed: value must be deref!
        // MemWrap(msg, a) = 1;
        MemWrap(*msg, a) = 1;
    }

    {
        Msg msg{0, 0};

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
    }

    return 0;
}
