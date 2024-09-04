/*************************************************************************
    > File Name: Geometry.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-20 23:27
************************************************************************/
#include <cmath>
#include <concepts>
#include <stdexcept>

#pragma once
template <typename T>
concept NumericType = std::integral<T> || std::floating_point<T>;

template<NumericType T, size_t N>
struct Vec {
    using value_type = T;
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
    constexpr T norm() const { return std::sqrt(*this * *this); }
};

template <NumericType T> struct Vec<T, 2> {
    using value_type = T;
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
    constexpr T norm() const { return std::sqrt(*this * *this); }
    constexpr Vec<T, 3> lift() { return {x, y, 0}; }
};

template <NumericType T> struct Vec<T, 3> {
    using value_type = T;
    T x{};
    T y{};
    T z{};
    constexpr T &operator[](const size_t i) {
        if (i >= 3)
            throw std::out_of_range("vector");
        return (i == 0) ? x : (i == 1) ? y : z;
    }

    constexpr T operator[](const size_t i) const {
        if (i >= 3)
            throw std::out_of_range("vector");
        return (i == 0) ? x : (i == 1) ? y : z;
   }
    constexpr T norm() const { return std::sqrt(*this * *this); }
};

using Point = Vec<int, 2>;
using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec2f = Vec<double, 2>;
using Vec3f = Vec<double, 3>;

template<NumericType T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<NumericType T, size_t N>
constexpr Vec<T, N> operator-(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

template<NumericType T, size_t N>
constexpr Vec<T, N> operator*(const Vec<T, N>& lhs, T c) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * c;
    }
    return res;
}

template<NumericType T, size_t N>
constexpr Vec<T, N> operator*(T c, const Vec<T, N>& rhs) {
    return rhs * c;
}

template<NumericType T, size_t N>
constexpr T operator*(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    T res {};
    for (int i = 0; i < N; ++i) {
        res += lhs[i] * rhs[i];
    }
    return res;
}

template<NumericType T>
constexpr Vec<T, 3> cross(const Vec<T, 3>& lhs, const Vec<T, 3>& rhs) {
    return {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}
