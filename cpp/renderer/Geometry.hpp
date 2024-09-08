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
#include <cstdlib>
#include <initializer_list>
#include <limits>
#include <span>
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
        T data[2];
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
        struct {
            T r, g, b;
        };
        T data[3];
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

template<NumericType T, NumericType R, size_t N>
constexpr auto operator+(const Vec<T, N>& lhs, const Vec<R, N>& rhs) {
    Vec<decltype(T{} + R{}), N> res;
    for (size_t i = 0; i < N; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<NumericType T, NumericType R, size_t N>
constexpr auto operator*(const Vec<T, N>& lhs, R c) {
    Vec<decltype(T{} * R{}), N> res;
    for (size_t i = 0; i < N; ++i) {
        res[i] = lhs[i] * c;
    }
    return res;
}

template<NumericType T, NumericType R, size_t N>
constexpr auto operator*(R c, const Vec<T, N>& rhs) {
    return rhs * c;
}

template<NumericType T,  size_t N>
constexpr auto operator-(const Vec<T, N>& lhs) {
    return lhs * -1;
}


template<NumericType T, NumericType R, size_t N>
constexpr auto operator-(const Vec<T, N>& lhs, const Vec<R, N>& rhs) {
    return lhs + -rhs;
}

template<NumericType T, NumericType R, size_t N>
constexpr auto operator*(const Vec<T, N>& lhs, const Vec<R, N>& rhs) {
    decltype(T{} * R{}) res {};
    for (size_t i = 0; i < N; ++i) {
        res += lhs[i] * rhs[i];
    }
    return res;
}

template <NumericType T, NumericType R>
constexpr auto cross(const Vec<T, 3> &lhs, const Vec<R, 3> &rhs) -> Vec<decltype(T{} * R{}), 3> {
    return {lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x};
}

namespace details {
template <NumericType T> constexpr T abs(T x) { return x >= 0 ? x : -x; }
template <NumericType T> constexpr bool eq(T x, T y) { return abs(x - y) <= std::numeric_limits<T>::epsilon(); }
template <NumericType T> constexpr T sqrt(T x, T curr, T prev) {
    return curr == prev ? curr : sqrt(x, (curr + x / curr) / T{2}, curr);
}
} // namespace details

template <NumericType T> constexpr T sqrtRoot(T x) {
    return x >= 0 ? details::sqrt(x, x, T{0}) : std::numeric_limits<T>::quiet_NaN();
}

namespace test {
static_assert(sqrtRoot(4) == 2);
static_assert(sqrtRoot(9) == 3);
static_assert(details::eq(sqrtRoot(5.5), 2.3452078799117149));
}

template <NumericType T, NumericType R, size_t N>
constexpr bool operator==(const Vec<T, N>& lhs, const Vec<R, N>& rhs) {
    for (size_t i = 0; i < N; ++i)
        if (!eq(lhs[i], rhs[i]))
            return false;
    return true;
}

template <NumericType T, NumericType R, size_t N>
constexpr bool operator!=(const Vec<T, N>& lhs, const Vec<R, N>& rhs) {
    return !(lhs == rhs);
}

template <NumericType T, size_t N> constexpr T norm(const Vec<T, N> &v) { return sqrtRoot(v * v); }
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

template<NumericType T, size_t M, size_t N>
struct Matrix {
    T data[M][N] {};
    constexpr Matrix() {}
    template <size_t... Dims>
    requires(sizeof...(Dims) == M && ((N == Dims) && ...))
    constexpr Matrix(const T (&... rows)[Dims]) {
        auto initRow = [this](int i, const T(&row)[N]) {
            for (size_t j = 0; j < std::size(row); ++j) {
                data[i][j] = row[j];
            }
        };
        int i = 0;
        (initRow(i++, rows), ...);
    }
};

template <typename T, size_t Dim, size_t... Dims>
requires((Dim == Dims) && ...)
Matrix(const T (&)[Dim], const T (&... rows)[Dims]) -> Matrix<T, sizeof...(Dims) + 1, Dim>;

template<NumericType T, NumericType R, size_t M, size_t N>
constexpr auto operator==(const Matrix<T, M, N>& lhs, const Matrix<R, M, N>& rhs) {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            if (!details::eq(lhs.data[i][j], rhs.data[i][j])) {
                return false;
            }
        }
    }
    return true;
}

template<NumericType T, NumericType R, size_t M, size_t N>
constexpr auto operator!=(const Matrix<T, M, N>& lhs, const Matrix<R, M, N>& rhs) {
    return !(lhs == rhs);
}

template<NumericType T, NumericType R, size_t M, size_t N>
constexpr auto operator*(const Matrix<T, M, N>& lhs, R c) {
    Matrix<decltype(T{} + R{}), M, N> res;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            res.data[i][j] = lhs.data[i][j] * c;
        }
    }
    return res;
}

template<NumericType T, NumericType R, size_t M, size_t N>
constexpr auto operator*(R c, const Matrix<T, M, N>& rhs) {
    return rhs * c;
}

template<NumericType T, size_t M, size_t N>
constexpr auto operator-(const Matrix<T, M, N>& lhs) {
    return T{-1} * lhs;
}

template<NumericType T, NumericType R, size_t M, size_t N>
constexpr auto operator+(const Matrix<T, M, N>& lhs, const Matrix<R, M, N>& rhs) {
    Matrix<decltype(T{} + R{}), M, N> res;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            res.data[i][j] = lhs.data[i][j] + rhs.data[i][j];
        }
    }
    return res;
}

template<NumericType T, NumericType R, size_t M, size_t N>
constexpr auto operator-(const Matrix<T, M, N>& lhs, const Matrix<R, M, N>& rhs) {
    return lhs + -rhs;
}

template<NumericType T, NumericType R, size_t M, size_t N, size_t P>
constexpr auto operator*(const Matrix<T, M, N>& lhs, const Matrix<R, N, P>& rhs) {
    Matrix<decltype(T{} * R{}), M, P> res;
    for (size_t i = 0; i < M; ++i) {
        for (size_t k = 0; k < N; ++k) {
            for (size_t j = 0; j < P; ++j) {
                res.data[i][j] += lhs.data[i][k] * rhs.data[k][j];
            }
        }
    }
    return res;
}

template<NumericType T, size_t M, size_t N>
constexpr auto transposition(const Matrix<T, M, N>& lhs) {
    Matrix<T, N, M> res;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            res.data[j][i] = lhs.data[i][j];
        }
    }
    return res;
}

namespace test {
static_assert([] {
    constexpr Matrix A = {
        {1, 2},
        {3, 4},
    };
    constexpr Matrix B = {
        {5, 6},
        {7, 8},
    };
    static_assert(A + B == Matrix{
                               {6, 8},
                               {10, 12},
                           });
    static_assert(B - A == Matrix{
                               {4, 4},
                               {4, 4},
                           });
    static_assert(A * 2 == Matrix{
                               {2, 4},
                               {6, 8},
                           });

    static_assert(transposition(Matrix{
                      {1, 2, 3},
                      {4, 5, 6},
                  }) == Matrix{{1, 4}, {2, 5}, {3, 6}});

    static_assert(Matrix{
                      {1, 2, 3},
                      {4, 5, 6},
                  } *
                      Matrix{
                          {7, 8},
                          {9, 10},
                          {11, 12},
                      } ==
                  Matrix{{58, 64}, {139, 154}});
    return true;
}());
} // namespace test

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using Point2i = Vec<int, 2>;
using Point3i = Vec<int, 3>;
using Point2f = Vec<float, 2>;
using Point3f = Vec<float, 3>;
using Color = Vec<uint8_t, 3>;
using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;

template <NumericType T, typename... Args>
requires(std::same_as<T, Args> && ...)
Vec(T, Args...) -> Vec<T, sizeof...(Args) + 1>;

