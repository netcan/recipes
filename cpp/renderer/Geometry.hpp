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
#include <type_traits>
#include <utility>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// math op
template <typename T>
concept NumericType = std::integral<T> || std::floating_point<T>;

namespace details {
template <NumericType T> constexpr T abs(T x) { return x >= 0 ? x : -x; }
template <NumericType T> constexpr bool eq(T x, T y) { return abs(x - y) <= std::numeric_limits<T>::epsilon(); }
template <NumericType T> constexpr T sqrt(T x, T curr, T prev) {
    return curr == prev ? curr : sqrt(x, (curr + x / curr) / T{2}, curr);
}
}

template <NumericType T> constexpr T sqrtRoot(T x) {
    return x >= 0 ? details::sqrt(x, x, T{0}) : std::numeric_limits<T>::quiet_NaN();
}

namespace test {
static_assert(sqrtRoot(4) == 2);
static_assert(sqrtRoot(9) == 3);
static_assert(details::eq(sqrtRoot(5.5), 2.3452078799117149));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// vec
template <NumericType T, size_t N>
struct Vec {
    T data[N] {};

    template <typename... Args> requires(sizeof...(Args) <= N) constexpr Vec(Args... args) : data(args...) {}

    template <typename Self> constexpr auto& operator[](this Self&& self, size_t i) { return self.data[i]; }

    // xyz
    template <typename Self> requires(N > 0) constexpr auto& x_(this Self&& self) { return self.data[0]; }
    template <typename Self> requires(N > 1) constexpr auto& y_(this Self&& self) { return self.data[1]; }
    template <typename Self> requires(N > 2) constexpr auto& z_(this Self&& self) { return self.data[2]; }

    // rgb
    template <typename Self> requires(N > 0) constexpr auto& r_(this Self&& self) { return self.data[0]; }
    template <typename Self> requires(N > 1) constexpr auto& g_(this Self&& self) { return self.data[1]; }
    template <typename Self> requires(N > 2) constexpr auto& b_(this Self&& self) { return self.data[2]; }

    // uvw
    template <typename Self> requires(N > 0) constexpr auto& u_(this Self&& self) { return self.data[0]; }
    template <typename Self> requires(N > 1) constexpr auto& v_(this Self&& self) { return self.data[1]; }
    template <typename Self> requires(N > 2) constexpr auto& w_(this Self&& self) { return self.data[2]; }

    constexpr T   norm() const { return sqrtRoot(*this * *this); }
    constexpr Vec normalize(T l = 1) const { return *this * (l / norm()); }
};

template <NumericType T, std::same_as<T>... Args>
Vec(T, Args...) -> Vec<T, sizeof...(Args) + 1>;

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
constexpr auto cross(const Vec<T, 3>& lhs, const Vec<R, 3>& rhs) -> Vec<decltype(T{} * R{}), 3> {
    return {lhs.y_() * rhs.z_() - lhs.z_() * rhs.y_(), lhs.z_() * rhs.x_() - lhs.x_() * rhs.z_(),
            lhs.x_() * rhs.y_() - lhs.y_() * rhs.x_()};
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// matrix
template<NumericType T, size_t M, size_t N>
struct Matrix {
    T data[M][N] {};
    constexpr Matrix() {}
    template <size_t... Dims> requires(sizeof...(Dims) == M && ((N == Dims) && ...))
    constexpr Matrix(const T (&... rows)[Dims]) {
        auto initRow = [this](int i, const T(&row)[N]) {
            for (size_t j = 0; j < std::size(row); ++j) {
                data[i][j] = row[j];
            }
        };
        int i = 0;
        (initRow(i++, rows), ...);
    }

    template<NumericType R, size_t V>
    constexpr void setCol(size_t j, const Vec<R, V>& col) {
        for (int i = 0; i < std::min(M, V); ++i) {
            data[i][j] = col[i];
        }
    }

    template<NumericType R, size_t V>
    constexpr void setRow(size_t i, const Vec<R, V>& row) {
        for (int j = 0; j < std::min(N, V); ++j) {
            data[i][j] = row[i];
        }
    }

    constexpr auto transposition() {
        Matrix<T, N, M> res;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                res.data[j][i] = data[i][j];
            }
        }
        return res;
    }
};

template <NumericType T, size_t Dim, size_t... Dims> requires((Dim == Dims) && ...)
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

    static_assert(Matrix{
                      {1, 2, 3},
                      {4, 5, 6},
                  }
                      .transposition() == Matrix{{1, 4}, {2, 5}, {3, 6}});

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
/// helpers
using Point2i = Vec<int, 2>;
using Point3i = Vec<int, 3>;
using Point2f = Vec<float, 2>;
using Point3f = Vec<float, 3>;
using Color = Vec<uint8_t, 3>;
using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
template<size_t M, size_t N>
using Matrixi = Matrix<int, M, N>;
template<size_t M, size_t N>
using Matrixf = Matrix<float, M, N>;

namespace details {
struct AnyType {
    template <typename T> operator T();
};
template <typename T> consteval size_t CountMember(auto&&... Args) {
    if constexpr (!requires { T(Args...); }) {
        return sizeof...(Args) - 1;
    } else {
        return CountMember<T>(Args..., AnyType{});
    }
}
}

template <typename R, NumericType T, size_t N>
constexpr R vcast(const Vec<T, N> &v) {
    constexpr size_t RN = details::CountMember<R>();
    return [&v]<size_t... Is>(std::index_sequence<Is...>) {
        return R (v[Is]...);
    }(std::make_index_sequence<std::min(N, RN)>{});
}

template<NumericType T, size_t N>
constexpr auto v2m(const Vec<T, N>& v) {
    Matrix<T, N, 1> res;
    for (size_t i = 0; i < N; ++i) {
        res.data[i][0] = v[i];
    }
    return res;
}

template<NumericType T, size_t N>
constexpr auto m2v(const Matrix<T, N, 1>& m) {
    Vec<T, N> res;
    for (size_t i = 0; i < N; ++i) {
        res[i] = m.data[i][0];
    }
    return res;
}
