/*************************************************************************
    > File Name: Datatable.cpp
    > Author: Netcan
    > Descripton: constexpr Datatable
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-14 21:16
************************************************************************/
#include <type_traits>
#include <algorithm>
#include <bitset>
#include <cstdio>

template <typename ...Ts>
struct TypeList {
    using type = TypeList<Ts...>;
    static constexpr size_t size = 0;
    template <typename T>
    using appendTo = typename TypeList<T>::type;
};

template <typename Head, typename ...Tails>
struct TypeList<Head, Tails...> {
    using type = TypeList<Head, Tails...>;
    using head = Head;
    using tails = TypeList<Tails...>;
    static constexpr size_t size = sizeof...(Tails) + 1;

    template <typename T>
    using appendTo = typename TypeList<Head, Tails..., T>::type;

    template <template<typename...> typename T>
    using exportTo = T<Head, Tails...>;
};

static_assert(std::is_same_v<TypeList<int, char>::type,
        TypeList<int>::appendTo<char>::type>);

template<typename IN,
    template<typename> typename P,
    typename S = TypeList<>,
    typename R = TypeList<>,
    typename = void>
struct Split {
    struct type {
        using satisfied = S;
        using rest = R;
    };
};

template<typename IN, template<typename> typename P, typename S, typename R>
class Split<IN, P, S, R, std::enable_if_t<P<typename IN::head>::value>> {
    using satisfied = typename S::template appendTo<typename IN::head>::type;
public:
    using type = typename Split<typename IN::tails, P, satisfied, R>::type;
};

template<typename IN,
    template<typename> typename P,
    typename S,
    typename R>
class Split<IN, P, S, R, std::enable_if_t<!P<typename IN::head>::value> >  {
    using rest = typename R::template appendTo<typename IN::head>::type;
public:
    using type = typename Split<typename IN::tails, P, S, rest>::type;
};


template<typename ES = TypeList<>, typename GS = TypeList<>, typename = void>
struct GroupEntries {
    using type = GS;
};

template<typename ES, typename GS>
class GroupEntries<ES, GS, std::void_t<typename ES::head>> {
    template<typename E>
    struct GroupPrediction {
        static constexpr bool value =
            ES::head::dim == E::dim &&
            sizeof(typename ES::head::type) == sizeof(typename E::type) &&
            alignof(typename ES::head::type) == alignof(typename E::type);
    };
    using group = typename Split<ES, GroupPrediction>::type;
    using satisfied = typename group::satisfied;
    using rest = typename group::rest;
public:
    using type = typename GroupEntries<rest, typename GS::template appendTo<satisfied>>::type;
};

template<typename IN, auto GROUP_N = 0, typename INDEX = TypeList<>, auto N = 0, typename = void>
struct RegionIndexer {
    using type = INDEX;
};

template<typename IN, typename INDEX, auto GROUP_N, auto N>
class RegionIndexer<IN, GROUP_N, INDEX, N, std::void_t<typename IN::head>> {
    struct entry {
        static constexpr auto key = IN::head::key;
        static constexpr auto id = (GROUP_N << 16) | N;
    };
    using entries = typename INDEX::template appendTo<entry>;
public:
    using type = typename RegionIndexer<typename IN::tails, GROUP_N, entries, N+1>::type;
};

template<typename IN, typename INDEX = TypeList<>, auto GROUP_N = 0, typename = void>
struct GroupIndexer {
    using type = INDEX;
};

template<typename IN, typename INDEX, auto GROUP_N>
class GroupIndexer<IN, INDEX, GROUP_N, std::void_t<typename IN::head>> {
    using entries = typename RegionIndexer<typename IN::head, GROUP_N>::type;
public:
    using type = typename GroupIndexer<typename IN::tails, entries, GROUP_N + 1>::type;
};

// <key, valuetype>
template<auto KEY, typename T, size_t DIM = 1>
struct Entry {
    static constexpr auto key = KEY;
    static constexpr size_t dim = DIM;
    static constexpr bool isArray = DIM > 1;
    using type = T;
};

template<auto KEY, typename T, size_t DIM>
struct Entry<KEY, T[DIM]>: Entry<KEY, T, DIM> { };


using AllEntries = TypeList<
    Entry<0, int>,
    Entry<1, char>,
    Entry<2, char>,
    Entry<3, short>,
    Entry<4, char[5]>,
    Entry<5, char[5]>,
    Entry<6, int> >;


int main(int argc, char** argv) {
    using GS = GroupEntries<AllEntries>::type;
    using INDEX = GroupIndexer<GS>::type;

    return 0;
}
