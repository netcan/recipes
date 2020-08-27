/*************************************************************************
    > File Name: PartiallyOrdering.cpp
    > Author: Netcan
    > Descripton: PartiallyOrdering, C++ morden design, Chapter 3.12
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-26 21:12
************************************************************************/
#include "Typelist.hpp"
#include <functional>

struct Widget              {};
struct ScrollBar:   Widget {};
struct Button:      Widget {};
struct GraphButton: Button {};

template<typename L, typename R>
struct DerivedCMP {
    static constexpr bool value = std::is_base_of_v<R, L>;
};

template<typename IN, template<typename, typename> class CMP, typename OUT = typename IN::head, typename = void>
struct SelectBest {
    using type = OUT;
};

template<typename IN, template<typename, typename> class CMP, typename OUT>
struct SelectBest<IN, CMP, OUT, std::void_t<typename IN::head>> {
    using type = typename SelectBest<typename IN::tails, CMP,
          std::conditional_t<
              CMP<typename IN::head, OUT>::value,
                typename IN::head,
                OUT>>::type;
};

template<typename IN>
using MostDerived = SelectBest<IN, DerivedCMP>;

template<typename IN>
using ReorderByMostDerived = Sort<IN, DerivedCMP>;

using PreOrderTL = TypeList<Widget, ScrollBar, Button, GraphButton>;
using ReorderTL = ReorderByMostDerived<PreOrderTL>::type;
using ExpectedTL = TypeList<ScrollBar, GraphButton, Button, Widget>;

static_assert(std::is_same_v<MostDerived<ReorderTL>::type, ScrollBar>);
static_assert(std::is_same_v<ReorderTL, ExpectedTL>);

template<int v>
using IntType = std::integer_sequence<int, v>;

template<typename L, typename R>
struct IntCmp;

template<int lhs, int rhs>
struct IntCmp<IntType<lhs>, IntType<rhs>> {
    static constexpr bool value = lhs < rhs;
};

using UnorderedNumberList = TypeList<
    IntType<69>,
    IntType<3>,
    IntType<72>,
    IntType<7>,
    IntType<53>,
    IntType<-67>,
    IntType<3>,
    IntType<6>,
    IntType<-23>
    >;

using OrderedNumberList = TypeList<
    IntType<-67>,
    IntType<-23>,
    IntType<3>,
    IntType<3>,
    IntType<6>,
    IntType<7>,
    IntType<53>,
    IntType<69>,
    IntType<72>>;

static_assert(std::is_same_v<
        OrderedNumberList,
        Sort<UnorderedNumberList, IntCmp>::type>);

int main(int argc, char** argv) {
    return 0;
}
