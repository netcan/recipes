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

#define GET_NTH_ARG(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, _16,         \
                    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,         \
                    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,         \
                    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, n, ...) n

#define GET_ARG_COUNT(...) GET_NTH_ARG(__VA_ARGS__,                     \
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
        48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
        16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1)

#define FOR_EACH(func, ...) GET_NTH_ARG(__VA_ARGS__,                                                            \
        FOR_EACH_64, FOR_EACH_63, FOR_EACH_62, FOR_EACH_61, FOR_EACH_60, FOR_EACH_59, FOR_EACH_58, FOR_EACH_57, \
        FOR_EACH_56, FOR_EACH_55, FOR_EACH_54, FOR_EACH_53, FOR_EACH_52, FOR_EACH_51, FOR_EACH_50, FOR_EACH_49, \
        FOR_EACH_48, FOR_EACH_47, FOR_EACH_46, FOR_EACH_45, FOR_EACH_44, FOR_EACH_43, FOR_EACH_42, FOR_EACH_41, \
        FOR_EACH_40, FOR_EACH_39, FOR_EACH_38, FOR_EACH_37, FOR_EACH_36, FOR_EACH_35, FOR_EACH_34, FOR_EACH_33, \
        FOR_EACH_32, FOR_EACH_31, FOR_EACH_30, FOR_EACH_29, FOR_EACH_28, FOR_EACH_27, FOR_EACH_26, FOR_EACH_25, \
        FOR_EACH_24, FOR_EACH_23, FOR_EACH_22, FOR_EACH_21, FOR_EACH_20, FOR_EACH_19, FOR_EACH_18, FOR_EACH_17, \
        FOR_EACH_16, FOR_EACH_15, FOR_EACH_14, FOR_EACH_13, FOR_EACH_12, FOR_EACH_11, FOR_EACH_10, FOR_EACH_9,  \
        FOR_EACH_8,  FOR_EACH_7,  FOR_EACH_6,  FOR_EACH_5,  FOR_EACH_4,  FOR_EACH_3,  FOR_EACH_2,  FOR_EACH_1 ) \
        (func, 0, __VA_ARGS__)

#define FOR_EACH_1(func, i, arg) func(i, arg);
#define FOR_EACH_2(func, i, arg, ...)  func(i, arg); FOR_EACH_1(func, i + 1, __VA_ARGS__)
#define FOR_EACH_3(func, i, arg, ...)  func(i, arg); FOR_EACH_2(func, i + 1, __VA_ARGS__)
#define FOR_EACH_4(func, i, arg, ...)  func(i, arg); FOR_EACH_3(func, i + 1, __VA_ARGS__)
#define FOR_EACH_5(func, i, arg, ...)  func(i, arg); FOR_EACH_4(func, i + 1, __VA_ARGS__)
#define FOR_EACH_6(func, i, arg, ...)  func(i, arg); FOR_EACH_5(func, i + 1, __VA_ARGS__)
#define FOR_EACH_7(func, i, arg, ...)  func(i, arg); FOR_EACH_6(func, i + 1, __VA_ARGS__)
#define FOR_EACH_8(func, i, arg, ...)  func(i, arg); FOR_EACH_7(func, i + 1, __VA_ARGS__)
#define FOR_EACH_9(func, i, arg, ...)  func(i, arg); FOR_EACH_8(func, i + 1, __VA_ARGS__)
#define FOR_EACH_10(func, i, arg, ...)  func(i, arg); FOR_EACH_9(func, i + 1, __VA_ARGS__)
#define FOR_EACH_11(func, i, arg, ...)  func(i, arg); FOR_EACH_10(func, i + 1, __VA_ARGS__)
#define FOR_EACH_12(func, i, arg, ...)  func(i, arg); FOR_EACH_11(func, i + 1, __VA_ARGS__)
#define FOR_EACH_13(func, i, arg, ...)  func(i, arg); FOR_EACH_12(func, i + 1, __VA_ARGS__)
#define FOR_EACH_14(func, i, arg, ...)  func(i, arg); FOR_EACH_13(func, i + 1, __VA_ARGS__)
#define FOR_EACH_15(func, i, arg, ...)  func(i, arg); FOR_EACH_14(func, i + 1, __VA_ARGS__)
#define FOR_EACH_16(func, i, arg, ...)  func(i, arg); FOR_EACH_15(func, i + 1, __VA_ARGS__)
#define FOR_EACH_17(func, i, arg, ...)  func(i, arg); FOR_EACH_16(func, i + 1, __VA_ARGS__)
#define FOR_EACH_18(func, i, arg, ...)  func(i, arg); FOR_EACH_17(func, i + 1, __VA_ARGS__)
#define FOR_EACH_19(func, i, arg, ...)  func(i, arg); FOR_EACH_18(func, i + 1, __VA_ARGS__)
#define FOR_EACH_20(func, i, arg, ...)  func(i, arg); FOR_EACH_19(func, i + 1, __VA_ARGS__)
#define FOR_EACH_21(func, i, arg, ...)  func(i, arg); FOR_EACH_20(func, i + 1, __VA_ARGS__)
#define FOR_EACH_22(func, i, arg, ...)  func(i, arg); FOR_EACH_21(func, i + 1, __VA_ARGS__)
#define FOR_EACH_23(func, i, arg, ...)  func(i, arg); FOR_EACH_22(func, i + 1, __VA_ARGS__)
#define FOR_EACH_24(func, i, arg, ...)  func(i, arg); FOR_EACH_23(func, i + 1, __VA_ARGS__)
#define FOR_EACH_25(func, i, arg, ...)  func(i, arg); FOR_EACH_24(func, i + 1, __VA_ARGS__)
#define FOR_EACH_26(func, i, arg, ...)  func(i, arg); FOR_EACH_25(func, i + 1, __VA_ARGS__)
#define FOR_EACH_27(func, i, arg, ...)  func(i, arg); FOR_EACH_26(func, i + 1, __VA_ARGS__)
#define FOR_EACH_28(func, i, arg, ...)  func(i, arg); FOR_EACH_27(func, i + 1, __VA_ARGS__)
#define FOR_EACH_29(func, i, arg, ...)  func(i, arg); FOR_EACH_28(func, i + 1, __VA_ARGS__)
#define FOR_EACH_30(func, i, arg, ...)  func(i, arg); FOR_EACH_29(func, i + 1, __VA_ARGS__)
#define FOR_EACH_31(func, i, arg, ...)  func(i, arg); FOR_EACH_30(func, i + 1, __VA_ARGS__)
#define FOR_EACH_32(func, i, arg, ...)  func(i, arg); FOR_EACH_31(func, i + 1, __VA_ARGS__)
#define FOR_EACH_33(func, i, arg, ...)  func(i, arg); FOR_EACH_32(func, i + 1, __VA_ARGS__)
#define FOR_EACH_34(func, i, arg, ...)  func(i, arg); FOR_EACH_33(func, i + 1, __VA_ARGS__)
#define FOR_EACH_35(func, i, arg, ...)  func(i, arg); FOR_EACH_34(func, i + 1, __VA_ARGS__)
#define FOR_EACH_36(func, i, arg, ...)  func(i, arg); FOR_EACH_35(func, i + 1, __VA_ARGS__)
#define FOR_EACH_37(func, i, arg, ...)  func(i, arg); FOR_EACH_36(func, i + 1, __VA_ARGS__)
#define FOR_EACH_38(func, i, arg, ...)  func(i, arg); FOR_EACH_37(func, i + 1, __VA_ARGS__)
#define FOR_EACH_39(func, i, arg, ...)  func(i, arg); FOR_EACH_38(func, i + 1, __VA_ARGS__)
#define FOR_EACH_40(func, i, arg, ...)  func(i, arg); FOR_EACH_39(func, i + 1, __VA_ARGS__)
#define FOR_EACH_41(func, i, arg, ...)  func(i, arg); FOR_EACH_40(func, i + 1, __VA_ARGS__)
#define FOR_EACH_42(func, i, arg, ...)  func(i, arg); FOR_EACH_41(func, i + 1, __VA_ARGS__)
#define FOR_EACH_43(func, i, arg, ...)  func(i, arg); FOR_EACH_42(func, i + 1, __VA_ARGS__)
#define FOR_EACH_44(func, i, arg, ...)  func(i, arg); FOR_EACH_43(func, i + 1, __VA_ARGS__)
#define FOR_EACH_45(func, i, arg, ...)  func(i, arg); FOR_EACH_44(func, i + 1, __VA_ARGS__)
#define FOR_EACH_46(func, i, arg, ...)  func(i, arg); FOR_EACH_45(func, i + 1, __VA_ARGS__)
#define FOR_EACH_47(func, i, arg, ...)  func(i, arg); FOR_EACH_46(func, i + 1, __VA_ARGS__)
#define FOR_EACH_48(func, i, arg, ...)  func(i, arg); FOR_EACH_47(func, i + 1, __VA_ARGS__)
#define FOR_EACH_49(func, i, arg, ...)  func(i, arg); FOR_EACH_48(func, i + 1, __VA_ARGS__)
#define FOR_EACH_50(func, i, arg, ...)  func(i, arg); FOR_EACH_49(func, i + 1, __VA_ARGS__)
#define FOR_EACH_51(func, i, arg, ...)  func(i, arg); FOR_EACH_50(func, i + 1, __VA_ARGS__)
#define FOR_EACH_52(func, i, arg, ...)  func(i, arg); FOR_EACH_51(func, i + 1, __VA_ARGS__)
#define FOR_EACH_53(func, i, arg, ...)  func(i, arg); FOR_EACH_52(func, i + 1, __VA_ARGS__)
#define FOR_EACH_54(func, i, arg, ...)  func(i, arg); FOR_EACH_53(func, i + 1, __VA_ARGS__)
#define FOR_EACH_55(func, i, arg, ...)  func(i, arg); FOR_EACH_54(func, i + 1, __VA_ARGS__)
#define FOR_EACH_56(func, i, arg, ...)  func(i, arg); FOR_EACH_55(func, i + 1, __VA_ARGS__)
#define FOR_EACH_57(func, i, arg, ...)  func(i, arg); FOR_EACH_56(func, i + 1, __VA_ARGS__)
#define FOR_EACH_58(func, i, arg, ...)  func(i, arg); FOR_EACH_57(func, i + 1, __VA_ARGS__)
#define FOR_EACH_59(func, i, arg, ...)  func(i, arg); FOR_EACH_58(func, i + 1, __VA_ARGS__)
#define FOR_EACH_60(func, i, arg, ...)  func(i, arg); FOR_EACH_59(func, i + 1, __VA_ARGS__)
#define FOR_EACH_61(func, i, arg, ...)  func(i, arg); FOR_EACH_60(func, i + 1, __VA_ARGS__)
#define FOR_EACH_62(func, i, arg, ...)  func(i, arg); FOR_EACH_61(func, i + 1, __VA_ARGS__)
#define FOR_EACH_63(func, i, arg, ...)  func(i, arg); FOR_EACH_62(func, i + 1, __VA_ARGS__)
#define FOR_EACH_64(func, i, arg, ...)  func(i, arg); FOR_EACH_63(func, i + 1, __VA_ARGS__)

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
        auto value() -> decltype(auto) {      \
            return (obj.STRIP(arg));          \
        }                                     \
        static constexpr const char* name() { \
            return STRING(STRIP(arg));        \
        }                                     \
    }                                         \

#define DEFINE_STRUCT(st, ...)                                              \
    struct st {                                                             \
        template <typename, size_t> struct FIELD;                           \
        static constexpr size_t _field_count_ = GET_ARG_COUNT(__VA_ARGS__); \
        FOR_EACH(FIELD_EACH, __VA_ARGS__)                                   \
    };                                                                      \

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
