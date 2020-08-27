/*************************************************************************
    > File Name: TypelistTest.cpp
    > Author: Netcan
    > Descripton: Test typelist
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-27 21:28
************************************************************************/

#include "Typelist.hpp"

static_assert(std::is_same_v<TypeList<int, char>,
        TypeList<int>::appendTo<char>>);


static_assert(std::is_same_v<Concat_t<TypeList<int, double>, TypeList<char, float>>,
        TypeList<int, double, char, float>>);

static_assert(std::is_same_v<Concat_t<TypeList<int>, TypeList<char, float>, TypeList<double>>,
        TypeList<int, char, float, double>>);

using SomeList = TypeList<char, float, double, int, char>;
using EmptyList = TypeList<>;

template<typename L, typename R>
struct SizeCmp {
    constexpr static bool value = sizeof(L) < sizeof(R);
};

static_assert(std::is_same_v<Sort_t<SomeList, SizeCmp>, TypeList<char, char, float, int, double>>);
static_assert(std::is_same_v<Unique_t<SomeList>, TypeList<char, float, double, int>>);
static_assert(Elem_v<SomeList, char>);
static_assert(!Elem_v<SomeList, long long>);
static_assert(!Elem_v<EmptyList, char>);

static_assert(std::is_same_v<Map_t<SomeList, std::add_pointer>,
        TypeList<char*, float*, double*, int*, char*>>);
static_assert(std::is_same_v<Map_t<EmptyList, std::add_pointer>,
        EmptyList>);

static_assert(std::is_same_v<Flatten_t<SomeList>, SomeList>);
static_assert(std::is_same_v<
        Flatten_t<TypeList<int, TypeList<char, TypeList<double>>>>
        , TypeList<int, char, double>>);

int main(int argc, char** argv) {
    return 0;
}
