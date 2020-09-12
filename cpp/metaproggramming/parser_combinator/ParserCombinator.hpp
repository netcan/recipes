/*************************************************************************
    > File Name: ParserCombinator.hpp
    > Author: Netcan
    > Descripton: ParserCombinator
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-12 19:55
************************************************************************/
#pragma once
#include <type_traits>
#include <utility>
#include <string>
#include <string_view>
#include <optional>

namespace ParserCombinator {
///////////////////////////////////////////////////////////////////////////////
// Parser a :: String -> Maybe (a, String)
using ParserInput = std::string_view;
template <typename T>
using ParserResult = std::optional<std::pair<T, ParserInput>>;

template<typename>
struct dump;

template<typename T>
class Parser {
    using TDecay = std::decay_t<T>;
public:
    using type = typename Parser<decltype(&TDecay::operator())>::type;
};

template<typename T>
using Parser_t = typename Parser<T>::type;

template<typename T, typename U>
struct Parser<auto (T::*)(ParserInput) -> ParserResult<U>>
{ using type = U; };

template<typename T, typename U>
struct Parser<auto (T::*)(ParserInput) const -> ParserResult<U>>:
    Parser<auto (T::*)(ParserInput) -> ParserResult<U>> {};


template<typename T>
class CoercionTrait {
    using TDecay = std::decay_t<T>;
public:
    using InputType  = typename CoercionTrait<decltype(&TDecay::operator())>::InputType;
    using ResultType = typename CoercionTrait<decltype(&TDecay::operator())>::ResultType;
};

template<typename U, typename V>
struct CoercionTrait<auto(*)(U)->V> {
    using InputType  = U;
    using ResultType = V;
};

template<typename T, typename U, typename V>
struct CoercionTrait<auto(T::*)(U)->V> {
    using InputType  = U;
    using ResultType = V;
};

template<typename T, typename U, typename V>
struct CoercionTrait<auto(T::*)(U) const -> V>:
    CoercionTrait<auto(T::*)(U) -> V> {};

///////////////////////////////////////////////////////////////////////////////
constexpr auto makeCharParser(char c) {
    // CharParser :: Parser Char
    return [=](ParserInput s) -> ParserResult<char> {
        if (s.empty() || c != s[0]) return std::nullopt;
        return std::make_pair(s[0], ParserInput(s.begin() + 1, s.size() - 1));
    };
};

constexpr auto makeStringParser(std::string_view str) {
    // StringParser :: Parser String
    return [=](ParserInput s) -> ParserResult<std::string_view> {
        if (s.empty() || s.find(str) != 0) return std::nullopt;
        return std::make_pair(str, ParserInput(s.begin() + str.size(), s.size() - str.size()));
    };
}

constexpr auto oneOf(std::string_view chars) {
    // OneOf :: Parser Char
    return [=](ParserInput s) -> ParserResult<char> {
        if (s.empty() || chars.find(s[0]) == std::string::npos) return std::nullopt;
        return std::make_pair(s[0], ParserInput(s.begin() + 1, s.size() - 1));
    };
}

constexpr auto noneOf(std::string_view chars) {
    // NoneOf :: Parser Char
    return [=](ParserInput s) -> ParserResult<char> {
        if (s.empty() || chars.find(s[0]) != std::string::npos) return std::nullopt;
        return std::make_pair(s[0], ParserInput(s.begin() + 1, s.size() - 1));
    };
}

// fmap :: (a -> b) -> Parser a -> Parser b
template<typename F, typename P,
    typename R = typename CoercionTrait<F>::ResultType>
constexpr auto fmap(F&& f, P&& p) {
    static_assert(std::is_same_v<
            typename CoercionTrait<F>::InputType,
            Parser_t<P> >, "type mismatch!");
    return [=](ParserInput s) -> ParserResult<R> {
        auto r = p(s);
        if (!r) return r;
        return std::make_pair(f(r->first), r->second);
    };
}

// bind :: Parser a -> (a -> ParserInput -> ParserResult a) -> Parser a
template<typename P, typename F>
constexpr auto bind(P&& p, F&& f) {
    using R = Parser_t<P>;
    static_assert(std::is_same_v<
            std::invoke_result_t<F, R, ParserInput>,
            ParserResult<R>>,
            "type mismatch!");
    return [=](ParserInput s) {
        auto r = p(s);
        if (!r) return r;
        return f(r->first, r->second);
    };
}

// operator| :: Parser a -> Parser a -> Parser a
template<typename P1, typename P2>
constexpr auto operator|(P1&& p1, P2&& p2) {
    return [=](ParserInput s) {
        auto r1 = p1(s);
        if (r1) return r1;
        auto r2 = p2(s);
        return r2;
    };
}

// combine :: Parser a -> Parser b -> (a -> b -> c) -> parser c
template<typename P1, typename P2, typename F,
    typename R = std::invoke_result_t<F, Parser_t<P1>, Parser_t<P2>>>
constexpr auto combine(P1&& p1, P2&& p2, F&& f) {
    return [=](ParserInput s) -> ParserResult<R> {
        auto r1 = p1(s);
        if (!r1) return r1;
        auto r2 = p2(r1->second);
        if (!r2) return r2;
        return std::make_pair(f(r1->first, r2->first), r2->second);
    };
};

// operator> :: Parser a -> Parser b -> Parser a
template<typename P1, typename P2>
constexpr auto operator>(P1&& p1, P2&& p2) {
    return combine(std::forward<P1>(p1),
                   std::forward<P2>(p2),
                   [](auto&& l, auto) { return l; });
};

// operator< :: Parser a -> Parser b -> Parser b
template<typename P1, typename P2>
constexpr auto operator<(P1&& p1, P2&& p2) {
    return combine(std::forward<P1>(p1),
                   std::forward<P2>(p2),
                   [](auto, auto&& r) { return r; });
};

// many :: Parser a -> b -> (b -> a -> b) -> Parser b
template<typename P, typename R, typename F>
constexpr auto many(P&& p, R&& init, F&& f) {
    static_assert(std::is_same_v<std::invoke_result_t<F, R, Parser_t<P>>, R>,
            "type mismatch!");
    return [=](ParserInput s) -> ParserResult<R> {
        ParserInput in = s;
        R acc = init;
        while (true) {
            auto r = p(in);
            if (!r) return std::make_pair(acc, in);
            acc = f(acc, r->first);
            in = r->second;
        }
    };
};

// separatedBy :: Parser a -> Parser x -> b -> (b -> a -> b) -> Parser b
template<typename P, typename X, typename R, typename F>
constexpr auto separatedBy(P&& p, X&& x, R&& init, F&& f) {
    static_assert(std::is_same_v<std::invoke_result_t<F, R, Parser_t<P>>, R>,
            "type mismatch!");
    return [=](ParserInput s) -> ParserResult<R> {
        ParserInput in = s;
        R acc = init;
        bool isX = false;
        while (true) {
            if (isX) {
                auto r = x(in);
                if (!r) return std::make_pair(acc, in);
                in = r->second;
            } else {
                auto r = p(in);
                if (!r) return std::make_pair(acc, in);
                acc = f(acc, r->first);
                in = r->second;
            }
            isX = !isX;
        }
    };
};

};
