/*************************************************************************
    > File Name: Typelist.hpp
    > Author: Netcan
    > Descripton: Typelist for meta programming
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-26 21:10
************************************************************************/
#pragma once
#include <type_traits>
#include <cstddef>

template<typename...>
struct dump;

struct Nil;

template <typename ...Ts>
struct TypeList {
    using type = TypeList<Ts...>;
    static constexpr size_t size = 0;

    template <typename ...T>
    using append = TypeList<T...>;

    template <typename T>
    using prepend = typename TypeList<T>::type;

    template <template<typename...> typename T>
    using exportTo = T<Ts...>;
};

template <typename Head, typename ...Tails>
struct TypeList<Head, Tails...> {
    using type = TypeList<Head, Tails...>;
    using head = Head;
    using tails = TypeList<Tails...>;
    static constexpr size_t size = sizeof...(Tails) + 1;

    template <typename ...Ts>
    using append = TypeList<Head, Tails..., Ts...>;

    template <typename T>
    using prepend = typename TypeList<T, Head, Tails...>::type;

    template <template<typename...> typename T>
    using exportTo = T<Head, Tails...>;
};

template<typename IN, template <typename> class F>
struct Map;

template<typename IN, template <typename> class F>
using Map_t = typename Map<IN, F>::type;

template<template <typename> class F, typename ...Ts>
struct Map<TypeList<Ts...>, F> {
    using type = TypeList<typename F<Ts>::type...>;
};

template<typename IN, template <typename> class P, typename OUT = TypeList<>>
struct Filter {
    using type = OUT;
};

template<typename IN, template <typename> class P>
using Filter_t = typename Filter<IN, P>::type;

template<template <typename> class P, typename OUT, typename H, typename ...Ts>
class Filter<TypeList<H, Ts...>, P, OUT> {
    using tails = TypeList<Ts...>;
public:
    using type = typename std::conditional_t<P<H>::value,
          Filter<tails, P, typename OUT::template append<H>>,
          Filter<tails, P, OUT>>::type;
};

template<typename IN, typename INIT, template<typename, typename> class OP>
struct FoldL {
    using type = INIT;
};

template<typename IN, typename INIT, template<typename, typename> class OP>
using FoldL_t = typename FoldL<IN, INIT, OP>::type;

template<typename ACC, template<typename, typename> class OP,
    typename H, typename ...Ts>
struct FoldL<TypeList<H, Ts...>, ACC, OP> {
    using type = FoldL_t<TypeList<Ts...>, typename OP<ACC, H>::type, OP>;
};

template<typename IN>
struct IsTypeList {
    constexpr static bool value = false;
};

template<typename IN>
constexpr bool IsTypeList_v = IsTypeList<IN>::value;

template<typename ...Ts>
struct IsTypeList<TypeList<Ts...>> {
    constexpr static bool value = true;
};

template<typename IN,
    template<typename> typename P,
    typename S = TypeList<>,
    typename R = TypeList<>,
    typename = void>
struct Partition {
    struct type {
        using satisfied = S;
        using rest = R;
    };
};

template<typename IN, template<typename> class P>
using Partition_t = typename Partition<IN, P>::type;

template<typename IN, template<typename> typename P, typename S, typename R>
class Partition<IN, P, S, R, std::enable_if_t<P<typename IN::head>::value>> {
    using satisfied = typename S::template append<typename IN::head>::type;
public:
    using type = typename Partition<typename IN::tails, P, satisfied, R>::type;
};

template<typename IN,
    template<typename> typename P,
    typename S,
    typename R>
class Partition<IN, P, S, R, std::enable_if_t<!P<typename IN::head>::value> >  {
    using rest = typename R::template append<typename IN::head>::type;
public:
    using type = typename Partition<typename IN::tails, P, S, rest>::type;
};

template<typename... IN>
struct Concat;

template<typename... IN>
using Concat_t = typename Concat<IN...>::type;

template<> struct Concat<> {
    using type = TypeList<>;
};
template<typename IN> struct Concat<IN> {
    using type = IN;
};

template<typename IN, typename IN2>
struct Concat<IN, IN2> {
    using type = typename IN2::template exportTo<IN::template append>::type;
};

template<typename IN, typename IN2, typename ...Rest>
struct Concat<IN, IN2, Rest...> {
    using type = Concat_t<Concat_t<IN, IN2>, Rest...>;
};



template<typename IN>
struct Flatten;

template<typename IN>
using Flatten_t = typename Flatten<IN>::type;

template<typename... Ts>
class Flatten<TypeList<Ts...>> {
    struct impl {
        template<typename ACC, typename E, typename = void>
        struct Append { using type = typename ACC::template append<E>; };
        template<typename ACC, typename E>
        struct Append<ACC, E, std::enable_if_t<IsTypeList_v<E>>> {
            using type = Concat_t<ACC, Flatten_t<E>>;
        };
    };

    template<typename IN, typename E>
    using Append = typename impl::template Append<IN, E>;

public:
    using type = FoldL_t<TypeList<Ts...>, TypeList<>, Append>;
};

template<typename IN, template<typename, typename> class CMP>
struct Sort {
    using type = TypeList<>;
};

template<typename IN, template<typename, typename> class CMP>
using Sort_t = typename Sort<IN, CMP>::type;

template<template<typename, typename> class CMP, typename H, typename ...Ts>
class Sort<TypeList<H, Ts...>, CMP> {
    template<typename E>
    struct LT { static constexpr bool value = CMP<E, H>::value; };
    using P = Partition_t<TypeList<Ts...>, LT>;
    using SmallerSorted = Sort_t<typename P::satisfied, CMP>;
    using BiggerSorted = Sort_t<typename P::rest, CMP>;
public:
    using type = Concat_t<typename SmallerSorted::template append<H>, BiggerSorted>;
};

template<typename IN, typename OUT=TypeList<>>
struct Unique {
    using type = OUT;
};

template<typename IN, typename E>
struct Elem {
    static constexpr bool value = false;
};

template<typename IN, typename E>
constexpr bool Elem_v = Elem<IN, E>::value;

template<typename E, typename ...Ts>
struct Elem<TypeList<Ts...>, E> {
    static constexpr bool value = (std::is_same_v<E, Ts> || ...);
};

template<typename IN>
using Unique_t = typename Unique<IN>::type;

template<typename OUT, typename H, typename... Ts>
class Unique<TypeList<H, Ts...>, OUT> {
    using tails = TypeList<Ts...>;
public:
    using type = typename std::conditional_t<Elem_v<OUT, H>,
          Unique<tails, OUT>,
          Unique<tails, typename OUT::template append<H>>>::type;
};

template<typename IN, template <typename> class F, typename = void>
struct FindBy {
    using type = Nil;
};

template<typename IN, template <typename> class F>
using FindBy_t = typename FindBy<IN, F>::type;

template<typename IN, template <typename> class F>
struct FindBy<IN, F, std::void_t<typename IN::head>> {
    using type = std::conditional_t<
        F<typename IN::head>::value,
        typename IN::head,
        typename FindBy<typename IN::tails, F>::type>;
};

template<typename A, typename B,
    template<typename, typename> class PAIR>
struct CrossProduct;

template<typename A, typename B, template<typename, typename> class PAIR>
using CrossProduct_t = typename CrossProduct<A, B, PAIR>::type;

template<typename A, typename B,
    template<typename, typename> class PAIR>
class CrossProduct {
    template<typename OUTER_LIST, typename TA>
    struct OuterAppend {
        template<typename INNER_LIST, typename TB>
        struct InnerAppend {
            using type = typename INNER_LIST::template append<PAIR<TA, TB>>;
        };
        using type = Concat_t<OUTER_LIST, FoldL_t<B, TypeList<>, InnerAppend>>;
    };

public:
    using type = FoldL_t<A, TypeList<>, OuterAppend>;
};

