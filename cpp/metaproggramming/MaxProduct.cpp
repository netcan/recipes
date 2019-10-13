// https://leetcode-cn.com/problems/integer-break/
#include <iostream>
#include <array>
#include <utility>

template<size_t ...Is>
struct Max;

template<>
struct Max<> {
    enum { value = 0 };
};

template<size_t n>
struct Max<n> {
    enum { value = n };
};

template<size_t n, size_t m>
struct Max<n, m> {
    enum { value = n >= m ? n : m };
};

template<size_t n, size_t m, size_t ...Is>
struct Max<n, m, Is...> {
    enum { value = Max<Max<n, m>::value, Max<Is...>::value>::value };
};

template<size_t n, typename U>
struct MaxProductImpl;

template<size_t n>
using MaxProduct = MaxProductImpl<n, std::make_index_sequence<n/2+1>>;

template<size_t n, size_t ...Is>
struct MaxProductImpl<n, std::index_sequence<0, Is...>> {
    enum { value =
        Max<
            Max< ( (n - Is) * Is)...>::value,
            Max< ( MaxProduct<(n - Is)>::value * Is)...>::value,
            Max< ( MaxProduct<(Is)>::value * (n - Is))...>::value,
            Max< ( MaxProduct<(Is)>::value * MaxProduct<(n - Is)>::value)...>::value
        >::value
    };
};

template<size_t ...Is>
static std::array<int, sizeof...(Is)> make_max_product_seq(std::index_sequence<Is...>) {
    return { MaxProduct<Is>::value... };
}

static const auto max_product_seq = make_max_product_seq(std::make_index_sequence<58>());

class Solution {
public:
    int integerBreak(int n) {
        return max_product_seq[n];
    }
};
