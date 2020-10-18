/*************************************************************************
    > File Name: FindShortestPath.cpp
    > Author: Netcan
    > Descripton: Find Shortest Path
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-09-14 21:45
************************************************************************/
#include "Typelist.hpp"
#include <utility>
#include <cassert>
#include <iostream>

template<typename F, typename T>
struct Edge {
    using From = F;
    using To = T;
};

template<typename Node = void>
struct EdgeTrait {
    template<typename Edge> struct IsFrom
    { constexpr static bool value = std::is_same_v<typename Edge::From, Node>; };
    template<typename Edge> struct IsTo
    { constexpr static bool value = std::is_same_v<typename Edge::To, Node>; };
    template<typename Edge>
    struct GetFrom { using type = typename Edge::From; };
    template<typename Edge>
    struct GetTo { using type = typename Edge::To; };
};


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
          typename OUT::template append<Edge<From, To>>>::type;
};

#define node(node) auto(*) (node)
#define link(link) link -> void

template<typename... Chains>
class Graph {
    using Edges = Unique_t<Concat_t<typename Chain<Chains>::type...>>;

///////////////////////////////////////////////////////////////////////////////
    template<typename FROM, typename TARGET,
        typename PATH = TypeList<>, typename = void>
    struct PathFinder;

    // Reach TARGET!
    template<typename TARGET, typename PATH>
    struct PathFinder<TARGET, TARGET, PATH>:
        PATH::template append<TARGET> { };

    // Skip cycle
    template<typename CURR_NODE, typename TARGET, typename PATH>
    struct PathFinder<CURR_NODE, TARGET, PATH,
        std::enable_if_t<Elem_v<PATH, CURR_NODE>>>: TypeList<> {}; // return empty path

    template<typename CURR_NODE, typename TARGET, typename PATH>
    class PathFinder<CURR_NODE, TARGET, PATH,
        std::enable_if_t<! std::is_same_v<CURR_NODE, TARGET>
            && !Elem_v<PATH, CURR_NODE>>> {
        using EdgesFrom = Filter_t<Edges,
                        EdgeTrait<CURR_NODE>::template IsFrom>;
        using NextNodes = Map_t<EdgesFrom, EdgeTrait<>::GetTo>;

        template<typename NEXT_NODE>
        struct GetPath: PathFinder<NEXT_NODE, TARGET,
            typename PATH::template append<CURR_NODE>> {};

        using AllPaths = Map_t<NextNodes, GetPath>;

        template<typename ACC, typename Path> struct MinPath:
            std::conditional_t<(ACC::size == 0 ||
                ((ACC::size > Path::size) && Path::size > 0)), Path, ACC> {};
    public:
        using type = FoldL_t<AllPaths, TypeList<>, MinPath>;
    };

///////////////////////////////////////////////////////////////////////////////
    template<typename NODE_TYPE>
    struct Path {
        const NODE_TYPE* path;
        size_t sz;
    };

    template<typename NODE, typename... NODEs>
    class PathStorage {
        using NODE_TYPE = std::decay_t<decltype(NODE::id)>;
        constexpr static NODE_TYPE
            pathStorage[] { NODE::id, NODEs::id... };
    public:
        constexpr static Path<NODE_TYPE> path {
            .path = pathStorage,
            .sz   = sizeof...(NODEs) + 1,
        };
    };

    using AllPairs = CrossProduct_t<
        Unique_t<Map_t<Edges, EdgeTrait<>::GetFrom>>,
        Unique_t<Map_t<Edges, EdgeTrait<>::GetTo>>,
        std::pair>;
    template<typename PAIR>
    struct GetPath {
        using type = std::pair<PAIR,
            typename PathFinder<typename PAIR::first_type,
                         typename PAIR::second_type>::type>;
    };
    template<typename PATH_PAIR>
    struct IsNonEmptyPath {
        constexpr static bool value = (PATH_PAIR::second_type::size > 0);
    };
    template<typename PATH_PAIR>
    struct SavePath {
        using type = std::pair<typename PATH_PAIR::first_type,
              typename PATH_PAIR::second_type::template exportTo<PathStorage>>;
    };
    using SavedPaths = Map_t<Filter_t<
        Map_t<AllPairs, GetPath>,
        IsNonEmptyPath>, SavePath>;

///////////////////////////////////////////////////////////////////////////////
    template<typename NODE_TYPE, typename FROM, typename TARGET, typename PATH>
    constexpr static bool matchPath(NODE_TYPE from, NODE_TYPE to,
            Path<NODE_TYPE>& result, std::pair<std::pair<FROM, TARGET>, PATH>) {
        if (FROM::id == from && TARGET::id == to) {
            result = PATH::path;
            return true;
        }
        return false;
    }

    template<typename NODE_TYPE, typename ...PATH_PAIRs>
    constexpr static void matchPath(NODE_TYPE from, NODE_TYPE to,
            Path<NODE_TYPE>& result, TypeList<PATH_PAIRs...>) {
        (matchPath(from, to, result, PATH_PAIRs{}) || ...);
    }

public:
    // export compile/run-time interface
    template<typename NODE_TYPE>
    constexpr static Path<NODE_TYPE> getPath(NODE_TYPE from, NODE_TYPE to) {
        Path<NODE_TYPE> result{};
        matchPath(from, to, result, SavedPaths{});
        return result;
    }
};

///////////////////////////////////////////////////////////////////////////////
template<char ID>
struct Node { constexpr static char id = ID; };

using A = Node<'A'>;
using B = Node<'B'>;
using C = Node<'C'>;
using D = Node<'D'>;
using E = Node<'E'>;

using g = Graph<
    link(node(A) -> node(B) -> node(C) -> node(D)),
    link(node(A) -> node(C)),   // test shortest path: A -> C -> D
    link(node(B) -> node(A)),   // test cycle
    link(node(A) -> node(E)) >; // test D -> E unreachable

static_assert(g::getPath('A', 'D').sz == 3); // compile-time test
static_assert(g::getPath('A', 'A').sz == 1);
static_assert(g::getPath('B', 'D').sz == 3);
static_assert(g::getPath('B', 'E').sz == 3);
static_assert(g::getPath('D', 'E').sz == 0);

int main(int argc, char** argv) {
    char from = 'A';
    char to = 'D';
    if (argc > 2) {
        from = argv[1][0]; // A
        to = argv[2][0]; // D
    }
    auto path = g::getPath(from, to); // runtime test
    std::cout << "from " << from << " to " << to
        << " path size: " << path.sz << std::endl;
    for (size_t i = 0; i < path.sz; ++i) {
        std::cout << path.path[i];
        if (i != path.sz - 1) {
            std::cout << "->";
        }
    }
    std::cout << std::endl;

    return 0;
}
