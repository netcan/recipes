/*************************************************************************
    > File Name: Geometry.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-20 23:27
************************************************************************/
#include <concepts>
#include <stdexcept>

#pragma once
template<std::integral T, size_t N>
struct Vec {
    T data[N]{};
    constexpr T& operator[](const size_t i) {
        if (i < N) {
            return data[i];
        }
        throw std::out_of_range("vector");
    }
    constexpr T operator[](const size_t i) const {
        if (i < N) {
            return data[i];
        }
        throw std::out_of_range("vector");
    }
};

template <std::integral T> struct Vec<T, 2> {
    T x{};
    T y{};
    constexpr T &operator[](const size_t i) {
        if (i >= 2)
            throw std::out_of_range("vector");
        return i == 0 ? x : y;
    }

    constexpr T operator[](const size_t i) const {
        if (i >= 2)
            throw std::out_of_range("vector");
        return i == 0 ? x : y;
   }
};

using Point = Vec<int, 2>;
using Vec2i = Vec<int, 2>;

template<std::integral T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<std::integral T, size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

template<std::integral T, size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& lhs, T c) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * c;
    }
    return res;
}

template<std::integral T, size_t N>
constexpr Vec<T, N> operator*(T c, const Vec<T, N>& lhs) {
    return lhs * c;
}

template<std::integral T>
constexpr T crossProd(const Vec<T, 2>& lhs, const Vec<T, 2>& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}
