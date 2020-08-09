/*************************************************************************
    > File Name: StaticRefl.cpp
    > Author: Netcan
    > Descripton: Static Reflection implementation
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-08 23:09
************************************************************************/
#include <iostream>
#include <tuple>
#include <typeinfo>

/*****************************************************
* Static Reflection Implementation
*****************************************************/
template<typename T>
inline constexpr auto StructMeta() {
    return std::make_tuple();
}

#define REFL(Struct, ...) template<>             \
    inline constexpr auto StructMeta<Struct>() { \
        using T = Struct;                        \
        return std::make_tuple(                  \
                __VA_ARGS__                      \
        );                                       \
    };                                           \

#define FIELD(field)                             \
    std::make_tuple(#field, &T::field)

template<typename T, typename Fields, typename F, size_t ...Is>
inline constexpr void foreach(T&& obj, Fields&& fields, F&& f, std::index_sequence<Is...>) {
    auto _ = { (f(
                std::get<0>(std::get<Is>(fields)),
                obj.*std::get<1>(std::get<Is>(fields))
                ), Is)..., 0ul };
}

template<typename T, typename F>
inline constexpr void foreach(T&& obj, F&& f) {
    constexpr auto fields = StructMeta<std::decay_t<T>>();
    foreach(std::forward<T>(obj),
            fields,
            std::forward<F>(f),
            std::make_index_sequence<std::tuple_size_v<decltype(fields)>>{});
}

/*****************************************************
* Static Reflection Test
*****************************************************/
struct ReflStructTest {
    int x;
    int y;
};

REFL(ReflStructTest,
        FIELD(x),
        FIELD(y));

struct Point {
    int x, y;
};

REFL(Point,
        FIELD(x),
        FIELD(y));

struct Rect {
    Point p1, p2;
    uint32_t color;
};

REFL(Rect,
        FIELD(p1),
        FIELD(p2),
        FIELD(color));


template<typename T>
void dumpObj(T&& obj, const char* fieldName = "", int depth = 0) {
    auto indent = [depth] {
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }
    };

    if constexpr(std::is_class_v<std::decay_t<T>>) {
        indent();
        std::cout << fieldName << (*fieldName ? ": {" : "{") << std::endl;
        foreach(obj, [depth](auto&& fieldName, auto&& value) {
            dumpObj(value, fieldName, depth + 1);
        });
        indent();
        std::cout << "}" << (depth == 0 ? "" : ",") << std::endl;
    } else {
        indent();
        std::cout << fieldName << ": " << obj << "," << std::endl;
    }
}

int main(int argc, char** argv) {
    Rect rect {
        {0, 0},
        {8, 9},
        12345678,
    };
	// {
	//    p1: {
	//       x: 0,
	//       y: 0,
	//    },
	//    p2: {
	//       x: 8,
	//       y: 9,
	//    },
	//    color: 12345678,
	// }
    dumpObj(rect);
    return 0;
}
