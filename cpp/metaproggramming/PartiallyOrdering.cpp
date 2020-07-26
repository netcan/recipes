/*************************************************************************
    > File Name: PartiallyOrdering.cpp
    > Author: Netcan
    > Descripton: PartiallyOrdering, C++ morden design, Chapter 3.12
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-26 21:12
************************************************************************/
#include "Typelist.hpp"

struct Widget              {};
struct ScrollBar:   Widget {};
struct Button:      Widget {};
struct GraphButton: Button {};

template <typename IN = TypeList<>, typename T = void, typename = void>
struct MostDerived {
    using type = T;
};

template <typename IN, typename T>
class MostDerived<IN, T, std::void_t<typename IN::head>> {
    using candidate = typename MostDerived<typename IN::tails, T>::type;
public:
    using type = std::conditional_t<
        std::is_base_of_v<candidate, typename IN::head>
        , typename IN::head
        , candidate>;
};

template <typename IN, typename = void>
struct ReorderByMostDerived {
    using type = IN;
};

template <typename IN>
class ReorderByMostDerived<IN, std::void_t<typename IN::head>> {
    using TheMostDerived = typename MostDerived<
        typename IN::tails
        , typename IN::head>::type;

    using L = typename Replace<
        typename IN::tails
        , TheMostDerived
        , typename IN::head>::type;

    using Rest = typename ReorderByMostDerived<L>::type;
public:
    using type = typename Rest::template prepend<TheMostDerived>;
};

using PreOrderTL = TypeList<Widget, ScrollBar, Button, GraphButton>;
using ReorderTL = ReorderByMostDerived<PreOrderTL>::type;

static_assert(std::is_same_v<
        GraphButton
        , MostDerived<PreOrderTL, PreOrderTL::head>::type
        >);

static_assert(std::is_same_v<
        ReorderTL
        , TypeList<GraphButton, ScrollBar, Button, Widget>::type>);

int main(int argc, char** argv) {

    return 0;
}
