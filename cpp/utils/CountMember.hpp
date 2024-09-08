/*************************************************************************
    > File Name: CountMember.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-09-08 15:10
************************************************************************/

#pragma once
#include <cstddef>

namespace utils {
namespace detail {
struct AnyType {
    template <typename T> operator T();
};
}
template <typename T> consteval size_t CountMember(auto&&... Args) {
    if constexpr (!requires { T(Args...); }) {
        return sizeof...(Args) - 1;
    } else {
        return CountMember<T>(Args..., detail::AnyType{});
    }
}

namespace test {
static_assert([] {
    struct Test {
        int a;
        int b;
        int c;
        int d;
    };
    static_assert(CountMember<Test>() == 4);
    return true;
}());
}
}
