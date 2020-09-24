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
struct Connection {
    using From = F;
    using To = T;
};

template<typename C = void>
struct ConnectionTrait {
    template<typename T> struct IsFrom
    { constexpr static bool value = std::is_same_v<typename T::From, C>; };
    template<typename T> struct IsTo
    { constexpr static bool value = std::is_same_v<typename T::To, C>; };
    template<typename T>
    struct GetFrom { using type = typename T::From; };
    template<typename T>
    struct GetTo { using type = typename T::To; };
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
          typename OUT::template appendTo<Connection<From, To>>>::type;
};

#define node(node) auto(*) (node)
#define link(link) link -> void

template<typename... Chains>
class Graph {
    using AllConnections = Unique_t<Concat_t<typename Chain<Chains>::type...>>;

///////////////////////////////////////////////////////////////////////////////
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
        using NextNodes = Map_t<EdgesFrom, ConnectionTrait<>::GetTo>;

    public:
        using type = typename PathFinder<NextNodes, TARGET,
              typename PATH::template appendTo<CURR_NODE>>::type;
    };

    // Skip cycle
    template<typename CURR_NODE, typename TARGET, typename PATH>
    struct PathFinder<CURR_NODE, TARGET, PATH,
        std::enable_if_t<!IsTypeList_v<CURR_NODE> &&
            Elem_v<PATH, CURR_NODE>>>: TypeList<> {}; // return empty path

    // Expansion NextNodes
    template<typename TARGET, typename PATH, typename ...CURR_NODE>
    class PathFinder<TypeList<CURR_NODE...>, TARGET, PATH> {
        using AllPaths = TypeList<
            typename PathFinder<CURR_NODE, TARGET, PATH>::type...
        >;
        template<typename ACC, typename Path> struct PathCmp {
            using type = std::conditional_t<(ACC::size == 0 ||
                    ((ACC::size > Path::size) && Path::size > 0)),
                  Path, ACC>;
        };
    public:
        using type = FoldL_t<AllPaths, TypeList<>, PathCmp>;
    };

    // Reach TARGET!
    template<typename TARGET, typename PATH>
    struct PathFinder<TARGET, TARGET, PATH> {
        using type = typename PATH::template appendTo<TARGET>;
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
        Unique_t<Map_t<AllConnections, ConnectionTrait<>::GetFrom>>,
        Unique_t<Map_t<AllConnections, ConnectionTrait<>::GetTo>>,
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
    using AllPaths = Map_t<Filter_t<
        Map_t<AllPairs, GetPath>,
        IsNonEmptyPath>, SavePath>;

///////////////////////////////////////////////////////////////////////////////
    template<typename NODE_TYPE, typename FROM, typename TO, typename PATH>
    constexpr static bool matchPath(NODE_TYPE from, NODE_TYPE to,
            Path<NODE_TYPE>& path, std::pair<std::pair<FROM, TO>, PATH>) {
        if (FROM::id == from && TO::id == to) {
            path = PATH::path;
            return true;
        }
        return false;
    }

    template<typename NODE_TYPE, typename ...PATH_PAIRs>
    constexpr static void matchPath(NODE_TYPE from, NODE_TYPE to,
            Path<NODE_TYPE>& path, TypeList<PATH_PAIRs...>) {
        (matchPath(from, to, path, PATH_PAIRs{}) || ...);
    }

public:
    // export compile/run-time interface
    template<typename NODE_TYPE>
    constexpr static Path<NODE_TYPE> getPath(NODE_TYPE from, NODE_TYPE to) {
        Path<NODE_TYPE> path{};
        matchPath(from, to, path, AllPaths{});
        return path;
    }
};

///////////////////////////////////////////////////////////////////////////////
template<size_t ID>
struct Node { constexpr static char id = ID; };

struct A: Node<'A'> {};
struct B: Node<'B'> {};
struct C: Node<'C'> {};
struct D: Node<'D'> {};
struct E: Node<'E'> {};
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
