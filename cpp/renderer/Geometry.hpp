/*************************************************************************
    > File Name: Geometry.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-08-20 23:27
************************************************************************/
#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>
#include <stdexcept>
#include <utility>

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
};

template <NumericType T> struct Vec<T, 2> {
    using value_type = T;
    union {
        struct {
            T x, y;
        };
        struct {
            T u, v;
        };
        struct {
            T w, h;
        };
    };
    constexpr Vec(T x = {}, T y = {}) : x{x}, y{y} {}
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

template <NumericType T> struct Vec<T, 3> {
    using value_type = T;
    union {
        struct {
            T x, y, z;
        };
        struct {
            T u, v, w;
        };
    };
    constexpr Vec(T x = {}, T y = {}, T z = {}) : x{x}, y{y}, z{z} {}
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
};

template<NumericType T, size_t N>
constexpr Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    Vec<T, N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<NumericType T, size_t N>
constexpr bool operator==(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    for (size_t i = 0; i < N; ++i)
        if (lhs[i] != rhs[i]) return false;
    return true;
}

template<NumericType T, size_t N>
constexpr bool operator!=(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    return !(lhs == rhs);
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

template <NumericType T, size_t N> constexpr T norm(const Vec<T, N> &v) { return std::sqrt(v * v); }
template <NumericType T, size_t N> constexpr Vec<T, N> normalize(const Vec<T, N> &v, T l = 1) {
    return v * (l / norm(v));
}

namespace details {
struct AnyType {
    template <typename T> operator T();
};
template <typename T> consteval size_t CountMember(auto &&...Args) {
    if constexpr (!requires { T{Args...}; }) {
        return sizeof...(Args) - 1;
    } else {
        return CountMember<T>(Args..., AnyType{});
    }
}
} // namespace details

template <typename R, NumericType T, size_t N>
constexpr R vec_cast(const Vec<T, N> &v) {
    constexpr size_t RN = details::CountMember<R>();
    return [&v]<size_t... Is>(std::index_sequence<Is...>) {
        return R (v[Is]...);
    }(std::make_index_sequence<std::min(N, RN)>{});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using Point2i = Vec<int, 2>;
using Point3i = Vec<int, 3>;
using Point2f = Vec<double, 2>;
using Point3f = Vec<double, 3>;
using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec2f = Vec<double, 2>;
using Vec3f = Vec<double, 3>;

template <NumericType T, typename... Args>
requires(std::same_as<T, Args> && ...)
Vec(T, Args...) -> Vec<T, sizeof...(Args) + 1>;

