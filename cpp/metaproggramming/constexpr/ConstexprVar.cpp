/*************************************************************************
    > File Name: constexpr_var.cpp
    > Author: Netcan
    > Descripton: constexpr constant
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-09 21:44
************************************************************************/
#include <cstdio>

template<char c>
constexpr bool is_digit = (c >= '0' && c <= '9');

template<char c>
constexpr bool is_digit_or_dot = (is_digit<c> || c == '.');

template<char c, char... cs>
constexpr bool is_sign_valid = ((c == '+' || c == '-') && sizeof...(cs) > 0)
                                || is_digit_or_dot<c>;

template<char... cs>
constexpr size_t number_of_dots = ((cs == '.' ? 1 : 0) + ... + 0);

template<char c, char... cs>
constexpr bool is_integer = is_sign_valid<c, cs...> &&
                            (is_digit<cs> && ...);

template<char... cs>
constexpr bool is_double = is_sign_valid<cs...> &&
                           ( (is_digit_or_dot<cs>) && ...) &&
                           number_of_dots<cs...> == 1;

template<char... cs>
constexpr bool is_number_valid = (is_integer<cs...> || is_double<cs...>);

template<size_t N>
constexpr size_t fibonacci = fibonacci<N - 1> + fibonacci<N - 2>;

template<>
constexpr size_t fibonacci<0> = 0;

template<>
constexpr size_t fibonacci<1> = 1;

int main(int argc, char** argv) {
    static_assert(! is_digit<'x'>);
    static_assert(is_digit<'0'>);

    static_assert(is_integer<'0'>);
    static_assert(!is_integer<'+'>);
    static_assert(!is_integer<'0', '.'>);
    static_assert(is_sign_valid<'+', '0'>);

    static_assert(is_number_valid<'1', '2', '3', '.', '4'>);
    static_assert(! is_number_valid<'a', 'b', 'c', 'd'>);

    static_assert(fibonacci<10> == 55);

    return 0;
}
