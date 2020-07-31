/*************************************************************************
    > File Name: StaticRefl.hpp
    > Author: Netcan
    > Descripton: StaticRefl implement
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-31 20:15
************************************************************************/
#ifndef STATIC_REFL_H
#define STATIC_REFL_H
#include <type_traits>
#include <iostream>
#include <utility>
#include <cstddef>

#define GET_NTH_ARG(_1, _2, _3, _4, n, ...) n
#define GET_ARG_COUNT(...) GET_NTH_ARG(__VA_ARGS__, 4, 3, 2, 1)

#define FOR_EACH(func, ...) \
    GET_NTH_ARG(__VA_ARGS__, FOR_EACH_4, FOR_EACH_3, FOR_EACH_2, FOR_EACH_1) \
    (func, 0, __VA_ARGS__) \

#define FOR_EACH_1(func, i, arg) func(i, arg);
#define FOR_EACH_2(func, i, arg, ...)  func(i, arg); FOR_EACH_1(func, i + 1, __VA_ARGS__)
#define FOR_EACH_3(func, i, arg, ...)  func(i, arg); FOR_EACH_2(func, i + 1, __VA_ARGS__)
#define FOR_EACH_4(func, i, arg, ...)  func(i, arg); FOR_EACH_3(func, i + 1, __VA_ARGS__)

#define STR(x) #x
#define STRING(x) STR(x)
#define PARE(...) __VA_ARGS__
#define EAT(...)
#define PAIR(x) PARE x // PAIR((int) x) => PARE(int) x => int x
#define STRIP(x) EAT x // STRIP((int) x) => EAT(int) x => x

#define FIELD_EACH(i, arg)                    \
    PAIR(arg);                                \
    template <typename T>                     \
    struct FIELD<T, i> {                      \
        T& obj;                               \
        FIELD(T& obj): obj(obj) {}            \
        auto value() -> decltype(auto) {                    \
            return (obj.STRIP(arg));            \
        }                                     \
        static constexpr const char* name() { \
            return STRING(STRIP(arg));        \
        }                                     \
    }                                         \

#define DEFINE_STRUCT(st, ...)                                              \
    struct st {                                                             \
        template <typename, size_t> struct FIELD;                       \
        static constexpr size_t _field_count_ = GET_ARG_COUNT(__VA_ARGS__); \
        FOR_EACH(FIELD_EACH, __VA_ARGS__)                                   \
    };                                                                       \

template<typename T, typename F, size_t... Is>
inline constexpr void forEach(T&& obj, F&& f, std::index_sequence<Is...>) {
    using TDECAY = std::decay_t<T>;
    (void(f(typename TDECAY::template FIELD<TDECAY, Is>(obj).name(),
            typename TDECAY::template FIELD<TDECAY, Is>(obj).value())), ...);
}

template<typename T, typename F>
inline constexpr void forEach(T&& obj, F&& f) {
    forEach(std::forward<T>(obj),
            std::forward<F>(f),
            std::make_index_sequence<std::decay_t<T>::_field_count_>{});
}

#endif
