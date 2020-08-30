/*************************************************************************
    > File Name: MetaMacro.hpp
    > Author: Netcan
    > Descripton: MetaMacro
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-30 08:56
************************************************************************/
#pragma once
#define GET_NTH_ARG(_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, _16,         \
                    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,         \
                    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,         \
                    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, n, ...) n

#define GET_ARG_COUNT(...) GET_NTH_ARG(__VA_ARGS__,                     \
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
        48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
        16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1)

#define FOR_EACH_1(func, i, arg)        func(i, arg)
#define FOR_EACH_2(func, i, arg, ...)   func(i, arg) FOR_EACH_1(func, i + 1, __VA_ARGS__)
#define FOR_EACH_3(func, i, arg, ...)   func(i, arg) FOR_EACH_2(func, i + 1, __VA_ARGS__)
#define FOR_EACH_4(func, i, arg, ...)   func(i, arg) FOR_EACH_3(func, i + 1, __VA_ARGS__)
#define FOR_EACH_5(func, i, arg, ...)   func(i, arg) FOR_EACH_4(func, i + 1, __VA_ARGS__)
#define FOR_EACH_6(func, i, arg, ...)   func(i, arg) FOR_EACH_5(func, i + 1, __VA_ARGS__)
#define FOR_EACH_7(func, i, arg, ...)   func(i, arg) FOR_EACH_6(func, i + 1, __VA_ARGS__)
#define FOR_EACH_8(func, i, arg, ...)   func(i, arg) FOR_EACH_7(func, i + 1, __VA_ARGS__)
#define FOR_EACH_9(func, i, arg, ...)   func(i, arg) FOR_EACH_8(func, i + 1, __VA_ARGS__)
#define FOR_EACH_10(func, i, arg, ...)  func(i, arg) FOR_EACH_9(func, i + 1, __VA_ARGS__)
#define FOR_EACH_11(func, i, arg, ...)  func(i, arg) FOR_EACH_10(func, i + 1, __VA_ARGS__)
#define FOR_EACH_12(func, i, arg, ...)  func(i, arg) FOR_EACH_11(func, i + 1, __VA_ARGS__)
#define FOR_EACH_13(func, i, arg, ...)  func(i, arg) FOR_EACH_12(func, i + 1, __VA_ARGS__)
#define FOR_EACH_14(func, i, arg, ...)  func(i, arg) FOR_EACH_13(func, i + 1, __VA_ARGS__)
#define FOR_EACH_15(func, i, arg, ...)  func(i, arg) FOR_EACH_14(func, i + 1, __VA_ARGS__)
#define FOR_EACH_16(func, i, arg, ...)  func(i, arg) FOR_EACH_15(func, i + 1, __VA_ARGS__)
#define FOR_EACH_17(func, i, arg, ...)  func(i, arg) FOR_EACH_16(func, i + 1, __VA_ARGS__)
#define FOR_EACH_18(func, i, arg, ...)  func(i, arg) FOR_EACH_17(func, i + 1, __VA_ARGS__)
#define FOR_EACH_19(func, i, arg, ...)  func(i, arg) FOR_EACH_18(func, i + 1, __VA_ARGS__)
#define FOR_EACH_20(func, i, arg, ...)  func(i, arg) FOR_EACH_19(func, i + 1, __VA_ARGS__)
#define FOR_EACH_21(func, i, arg, ...)  func(i, arg) FOR_EACH_20(func, i + 1, __VA_ARGS__)
#define FOR_EACH_22(func, i, arg, ...)  func(i, arg) FOR_EACH_21(func, i + 1, __VA_ARGS__)
#define FOR_EACH_23(func, i, arg, ...)  func(i, arg) FOR_EACH_22(func, i + 1, __VA_ARGS__)
#define FOR_EACH_24(func, i, arg, ...)  func(i, arg) FOR_EACH_23(func, i + 1, __VA_ARGS__)
#define FOR_EACH_25(func, i, arg, ...)  func(i, arg) FOR_EACH_24(func, i + 1, __VA_ARGS__)
#define FOR_EACH_26(func, i, arg, ...)  func(i, arg) FOR_EACH_25(func, i + 1, __VA_ARGS__)
#define FOR_EACH_27(func, i, arg, ...)  func(i, arg) FOR_EACH_26(func, i + 1, __VA_ARGS__)
#define FOR_EACH_28(func, i, arg, ...)  func(i, arg) FOR_EACH_27(func, i + 1, __VA_ARGS__)
#define FOR_EACH_29(func, i, arg, ...)  func(i, arg) FOR_EACH_28(func, i + 1, __VA_ARGS__)
#define FOR_EACH_30(func, i, arg, ...)  func(i, arg) FOR_EACH_29(func, i + 1, __VA_ARGS__)
#define FOR_EACH_31(func, i, arg, ...)  func(i, arg) FOR_EACH_30(func, i + 1, __VA_ARGS__)
#define FOR_EACH_32(func, i, arg, ...)  func(i, arg) FOR_EACH_31(func, i + 1, __VA_ARGS__)
#define FOR_EACH_33(func, i, arg, ...)  func(i, arg) FOR_EACH_32(func, i + 1, __VA_ARGS__)
#define FOR_EACH_34(func, i, arg, ...)  func(i, arg) FOR_EACH_33(func, i + 1, __VA_ARGS__)
#define FOR_EACH_35(func, i, arg, ...)  func(i, arg) FOR_EACH_34(func, i + 1, __VA_ARGS__)
#define FOR_EACH_36(func, i, arg, ...)  func(i, arg) FOR_EACH_35(func, i + 1, __VA_ARGS__)
#define FOR_EACH_37(func, i, arg, ...)  func(i, arg) FOR_EACH_36(func, i + 1, __VA_ARGS__)
#define FOR_EACH_38(func, i, arg, ...)  func(i, arg) FOR_EACH_37(func, i + 1, __VA_ARGS__)
#define FOR_EACH_39(func, i, arg, ...)  func(i, arg) FOR_EACH_38(func, i + 1, __VA_ARGS__)
#define FOR_EACH_40(func, i, arg, ...)  func(i, arg) FOR_EACH_39(func, i + 1, __VA_ARGS__)
#define FOR_EACH_41(func, i, arg, ...)  func(i, arg) FOR_EACH_40(func, i + 1, __VA_ARGS__)
#define FOR_EACH_42(func, i, arg, ...)  func(i, arg) FOR_EACH_41(func, i + 1, __VA_ARGS__)
#define FOR_EACH_43(func, i, arg, ...)  func(i, arg) FOR_EACH_42(func, i + 1, __VA_ARGS__)
#define FOR_EACH_44(func, i, arg, ...)  func(i, arg) FOR_EACH_43(func, i + 1, __VA_ARGS__)
#define FOR_EACH_45(func, i, arg, ...)  func(i, arg) FOR_EACH_44(func, i + 1, __VA_ARGS__)
#define FOR_EACH_46(func, i, arg, ...)  func(i, arg) FOR_EACH_45(func, i + 1, __VA_ARGS__)
#define FOR_EACH_47(func, i, arg, ...)  func(i, arg) FOR_EACH_46(func, i + 1, __VA_ARGS__)
#define FOR_EACH_48(func, i, arg, ...)  func(i, arg) FOR_EACH_47(func, i + 1, __VA_ARGS__)
#define FOR_EACH_49(func, i, arg, ...)  func(i, arg) FOR_EACH_48(func, i + 1, __VA_ARGS__)
#define FOR_EACH_50(func, i, arg, ...)  func(i, arg) FOR_EACH_49(func, i + 1, __VA_ARGS__)
#define FOR_EACH_51(func, i, arg, ...)  func(i, arg) FOR_EACH_50(func, i + 1, __VA_ARGS__)
#define FOR_EACH_52(func, i, arg, ...)  func(i, arg) FOR_EACH_51(func, i + 1, __VA_ARGS__)
#define FOR_EACH_53(func, i, arg, ...)  func(i, arg) FOR_EACH_52(func, i + 1, __VA_ARGS__)
#define FOR_EACH_54(func, i, arg, ...)  func(i, arg) FOR_EACH_53(func, i + 1, __VA_ARGS__)
#define FOR_EACH_55(func, i, arg, ...)  func(i, arg) FOR_EACH_54(func, i + 1, __VA_ARGS__)
#define FOR_EACH_56(func, i, arg, ...)  func(i, arg) FOR_EACH_55(func, i + 1, __VA_ARGS__)
#define FOR_EACH_57(func, i, arg, ...)  func(i, arg) FOR_EACH_56(func, i + 1, __VA_ARGS__)
#define FOR_EACH_58(func, i, arg, ...)  func(i, arg) FOR_EACH_57(func, i + 1, __VA_ARGS__)
#define FOR_EACH_59(func, i, arg, ...)  func(i, arg) FOR_EACH_58(func, i + 1, __VA_ARGS__)
#define FOR_EACH_60(func, i, arg, ...)  func(i, arg) FOR_EACH_59(func, i + 1, __VA_ARGS__)
#define FOR_EACH_61(func, i, arg, ...)  func(i, arg) FOR_EACH_60(func, i + 1, __VA_ARGS__)
#define FOR_EACH_62(func, i, arg, ...)  func(i, arg) FOR_EACH_61(func, i + 1, __VA_ARGS__)
#define FOR_EACH_63(func, i, arg, ...)  func(i, arg) FOR_EACH_62(func, i + 1, __VA_ARGS__)
#define FOR_EACH_64(func, i, arg, ...)  func(i, arg) FOR_EACH_63(func, i + 1, __VA_ARGS__)

#define STR(x) #x
#define CONCATE(x, y) x ## y
#define STRING(x) STR(x)
#define PARE(...) __VA_ARGS__
#define EAT(...)
#define PAIR(x) PARE x // PAIR((int) x) => PARE(int) x => int x
#define STRIP(x) EAT x // STRIP((int) x) => EAT(int) x => x
#define PASTE(x, y) CONCATE(x, y)
