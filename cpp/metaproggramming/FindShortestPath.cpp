/*************************************************************************
    > File Name: FindShortestPath.cpp
    > Author: Netcan
    > Descripton: Find Shortest Path
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-14 21:45
************************************************************************/
#include "Typelist.hpp"

template<typename F, typename T>
struct Connection {
    using From = F;
    using To = T;
};

template<typename C>
struct ConnectionTrait {
    template<typename T> struct IsFrom
    { constexpr static bool value = std::is_same_v<typename T::From, C>; };
    template<typename T> struct IsTo
    { constexpr static bool value = std::is_same_v<typename T::To, C>; };
};

template<typename C>
struct GetFrom { using type = typename C::From; };
template<typename C>
struct GetTo { using type = typename C::To; };

template<typename T, typename OUT = TypeList<>>
struct Chain;

template<typename F, typename OUT>
struct Chain<auto(*)(F) -> void, OUT> {
    using From = F;
    using type = OUT;
};

template<typename F, typename T, typename OUT>
struct Chain<auto(*)(F) -> T, OUT> {
private:
    using To = typename Chain<T, OUT>::From;
public:
    using From = F;
    using type = typename Chain<T,
          typename OUT::template appendTo<Connection<From, To>>>::type;
};

#define __node(node) auto(*) (node)
#define __link(link) link -> void

template<typename... Chains>
class Graph {
public:
    using AllConnections = Unique_t<Concat_t<typename Chain<Chains>::type...>>;

    template<typename F, typename TARGET,
        typename PATH = TypeList<>, typename = void>
    struct PathFinder;

    template<typename CURR_NODE, typename TARGET, typename PATH>
    class PathFinder<CURR_NODE, TARGET, PATH,
        std::enable_if_t<! std::is_same_v<CURR_NODE, TARGET> &&
            !IsTypeList_v<CURR_NODE> &&
            !Elem_v<PATH, CURR_NODE>>> {
        using EdgesFrom = Filter_t<AllConnections,
                        ConnectionTrait<CURR_NODE>::template IsFrom>;
        using NextNodes = Map_t<EdgesFrom, GetTo>;

    public:
        using type = typename PathFinder<NextNodes, TARGET,
              typename PATH::template appendTo<CURR_NODE>>::type;
    };

    // Skip cycle
    template<typename CURR_NODE, typename TARGET, typename PATH>
    class PathFinder<CURR_NODE, TARGET, PATH,
        std::enable_if_t<! std::is_same_v<CURR_NODE, TARGET> &&
            !IsTypeList_v<CURR_NODE> &&
            Elem_v<PATH, CURR_NODE>>> {
    public:
        using type = TypeList<>;
    };

    // Expansion NextNodes
    template<typename TARGET, typename PATH, typename ...CURR_NODE>
    class PathFinder<TypeList<CURR_NODE...>, TARGET, PATH> {
        using AllPaths = TypeList<
            typename PathFinder<CURR_NODE, TARGET, PATH>::type...
        >;
        template<typename ACC, typename Path> struct PathCmp {
            using type = std::conditional_t<ACC::size == 0, Path,
                std::conditional_t<(ACC::size > Path::size && Path::size != 0),
                Path, ACC>>;
        };
    public:
        using type = FoldL_t<AllPaths, TypeList<>, PathCmp>;
    };

    // Reach TARGET!
    template<typename TARGET, typename PATH>
    struct PathFinder<TARGET, TARGET, PATH> {
        using type = typename PATH::template appendTo<TARGET>;
    };

public:
    template<typename F, typename TARGET>
    using PathFinder_t = typename PathFinder<F, TARGET>::type;
};

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};
using g = Graph<
    __link(__node(A) -> __node(B) -> __node(C) -> __node(D)),
    __link(__node(A) -> __node(C)),
    __link(__node(A) -> __node(E)) >;

static_assert(g::PathFinder_t<A, D>::size == 3);
static_assert(g::PathFinder_t<A, A>::size == 1);
static_assert(g::PathFinder_t<B, D>::size == 3);
static_assert(g::PathFinder_t<B, E>::size == 0);

int main(int argc, char** argv) {

    return 0;
}
