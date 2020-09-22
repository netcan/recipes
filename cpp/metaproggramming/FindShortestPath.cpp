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

#define __node(node) auto(*) (node)
#define __link(link) link -> void

template<typename... Chains>
class Graph {
private:
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
        using NextNodes = Map_t<EdgesFrom, ConnectionTrait<>::GetTo>;

    public:
        using type = typename PathFinder<NextNodes, TARGET,
              typename PATH::template appendTo<CURR_NODE>>::type;
    };

    // Skip cycle
    template<typename CURR_NODE, typename TARGET, typename PATH>
    struct PathFinder<CURR_NODE, TARGET, PATH,
        std::enable_if_t<! std::is_same_v<CURR_NODE, TARGET> &&
            !IsTypeList_v<CURR_NODE> &&
            Elem_v<PATH, CURR_NODE>>>: TypeList<> {};

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

public:
    // export compile-time interface
    template<typename F, typename TARGET>
    using PathFinder_t = typename PathFinder<F, TARGET>::type;

private:
    struct Path {
        const size_t* path;
        size_t sz;
    };
    template<typename... NODEs>
    class PathStorage {
        constexpr static size_t pathStorage[] { NODEs::id... };
    public:
        constexpr static Path path {
            .path = pathStorage,
            .sz   = sizeof...(NODEs)
        };
    };

    template<typename T>
    struct IsDifferPair {
        constexpr static bool value = !std::is_same_v<
            typename T::first_type, typename T::second_type>;
    };
    using AllPairs = Filter_t<CrossProduct_t<
        Unique_t<Map_t<AllConnections, ConnectionTrait<>::GetFrom>>,
        Unique_t<Map_t<AllConnections, ConnectionTrait<>::GetTo>>,
        std::pair>, IsDifferPair>;
    template<typename PAIR>
    struct GetPath {
        using type = std::pair<PAIR,
            PathFinder_t<typename PAIR::first_type,
                         typename PAIR::second_type>>;
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

private:
    template<typename FROM, typename TO, typename PATH>
    static bool matchPath(size_t from, size_t to,
            Path& path, std::pair<std::pair<FROM, TO>, PATH>) {
        if (FROM::id == from && TO::id == to) {
            path = PATH::path;
            return true;
        }
        return false;
    }

    template<typename ...PATH_PAIRs>
    static void matchPath(size_t from, size_t to,
            Path& path, TypeList<PATH_PAIRs...>) {
        (matchPath(from, to, path, PATH_PAIRs{}) || ...);
    }

public:
    // export run-time interface
    static Path getPath(size_t from, size_t to) {
        Path path{};
        matchPath(from, to, path, AllPaths{});
        return path;
    }
};

///////////////////////////////////////////////////////////////////////////////
template<size_t ID>
struct Node { constexpr static size_t id = ID; };

struct A: Node<0> {};
struct B: Node<1> {};
struct C: Node<2> {};
struct D: Node<3> {};
struct E: Node<4> {};
using g = Graph<
    __link(__node(A) -> __node(B) -> __node(C) -> __node(D)),
    __link(__node(A) -> __node(C)),
    __link(__node(B) -> __node(A)),
    __link(__node(A) -> __node(E)) >;

static_assert(g::PathFinder_t<A, D>::size == 3);
static_assert(g::PathFinder_t<A, A>::size == 1);
static_assert(g::PathFinder_t<B, D>::size == 3);
static_assert(g::PathFinder_t<B, E>::size == 3);
static_assert(g::PathFinder_t<D, E>::size == 0);

int main(int argc, char** argv) {
    volatile size_t from = 0; // A
    volatile size_t to = 3; // D
    auto path = g::getPath(from, to);
    assert(path.sz == 3);
    // 0(A) -> 2(C) -> 3(D)
    for (size_t i = 0; i < path.sz; ++i) {
        std::cout << path.path[i];
        if (i != path.sz - 1) {
            std::cout << "->";
        }
    }
    std::cout << std::endl;

    return 0;
}
