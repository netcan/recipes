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

template<typename T>
struct Return { using type = T; };

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

template<typename G>
struct Not;

template<template<typename...> class G, typename... Args>
struct Not<G<Args...>>: std::bool_constant<!G<Args...>::value> { };

template<typename IN, template <typename> class F>
struct Map;

template<typename IN, template <typename> class F>
using Map_t = typename Map<IN, F>::type;

template<template <typename> class F, typename ...Ts>
struct Map<TypeList<Ts...>, F>: TypeList<typename F<Ts>::type...> {};

template<typename IN, template <typename> class P, typename OUT = TypeList<>>
struct Filter: OUT {};

template<typename IN, template <typename> class P>
using Filter_t = typename Filter<IN, P>::type;

template<template <typename> class P, typename OUT, typename H, typename ...Ts>
struct Filter<TypeList<H, Ts...>, P, OUT>:
    std::conditional_t<P<H>::value,
        Filter<TypeList<Ts...>, P, typename OUT::template append<H>>,
        Filter<TypeList<Ts...>, P, OUT>> { };

template<typename IN, typename INIT, template<typename, typename> class OP>
struct FoldL: Return<INIT> {};

template<typename IN, typename INIT, template<typename, typename> class OP>
using FoldL_t = typename FoldL<IN, INIT, OP>::type;

template<typename ACC, template<typename, typename> class OP,
    typename H, typename ...Ts>
struct FoldL<TypeList<H, Ts...>, ACC, OP>:
    FoldL<TypeList<Ts...>, typename OP<ACC, H>::type, OP> {};

template<typename IN,
    template<typename> typename P,
    typename S = TypeList<>,
    typename R = TypeList<>>
struct Partition {
    struct type {
        using satisfied = S;
        using rest = R;
    };
};

template<typename IN, template<typename> class P>
using Partition_t = typename Partition<IN, P>::type;

template<typename H, typename ...Ts, template<typename> typename P, typename S, typename R>
struct Partition<TypeList<H, Ts...>, P, S, R>: std::conditional_t<P<H>::value,
      Partition<TypeList<Ts...>, P, typename S::template append<H>, R>,
      Partition<TypeList<Ts...>, P, S, typename R::template append<H>>> {};

template<typename... IN>
struct Concat;

template<typename... IN>
using Concat_t = typename Concat<IN...>::type;

template<> struct Concat<>: TypeList<> { };

template<typename IN> struct Concat<IN>: IN { };

template<typename IN, typename IN2>
struct Concat<IN, IN2>: IN2::template exportTo<IN::template append> { };

template<typename IN, typename IN2, typename ...Rest>
struct Concat<IN, IN2, Rest...>: Concat_t<Concat_t<IN, IN2>, Rest...> { };

template<typename IN>
class Flatten {
    struct impl {
        template<typename ACC, typename E>
        struct Append: ACC::template append<E> {};

        template<typename ACC, typename ...Ts>
        struct Append<ACC, TypeList<Ts...>>:
            Concat_t<ACC, typename Flatten<TypeList<Ts...>>::type> {};
    };

    template<typename ACC, typename E>
    using Append = typename impl::template Append<ACC, E>;

public:
    using type = FoldL_t<IN, TypeList<>, Append>;
};

template<typename IN>
using Flatten_t = typename Flatten<IN>::type;

template<typename IN, template<typename, typename> class CMP>
struct Sort: TypeList<> {};

template<typename IN, template<typename, typename> class CMP>
using Sort_t = typename Sort<IN, CMP>::type;

template<template<typename, typename> class CMP, typename H, typename ...Ts>
class Sort<TypeList<H, Ts...>, CMP> {
    template<typename E> struct LT: CMP<E, H> { };
    using P = Partition_t<TypeList<Ts...>, LT>;
    using SmallerSorted = Sort_t<typename P::satisfied, CMP>;
    using BiggerSorted = Sort_t<typename P::rest, CMP>;
public:
    using type = Concat_t<typename SmallerSorted::template append<H>, BiggerSorted>;
};

template<typename IN, typename E>
struct Elem: std::false_type { };

template<typename IN, typename E>
constexpr bool Elem_v = Elem<IN, E>::value;

template<typename E, typename ...Ts>
struct Elem<TypeList<Ts...>, E>:
    std::bool_constant<(std::is_same_v<E, Ts> || ...)> { };

template<typename IN>
class Unique {
    template<typename ACC, typename E>
    struct Append: std::conditional_t<Elem_v<ACC, E>,
        ACC, typename ACC::template append<E>> {};

public:
    using type = FoldL_t<IN, TypeList<>, Append>;
};

template<typename IN>
using Unique_t = typename Unique<IN>::type;

template<typename IN, template <typename> class F>
struct FindBy: Return<Nil> {};

template<typename IN, template <typename> class F>
using FindBy_t = typename FindBy<IN, F>::type;

template<typename H, typename ...Ts, template <typename> class F>
struct FindBy<TypeList<H, Ts...>, F>: std::conditional_t<F<H>::value,
    Return<H>, FindBy<TypeList<Ts...>, F>> {};

template<typename A, typename B,
    template<typename, typename> class PAIR>
struct CrossProduct;

template<typename A, typename B, template<typename, typename> class PAIR>
using CrossProduct_t = typename CrossProduct<A, B, PAIR>::type;

template<typename A, typename B, template<typename, typename> class PAIR>
class CrossProduct {
    template<typename RESULT_OUTTER, typename TA>
    struct OuterAppend {
        template<typename RESULT_INNER, typename TB>
        struct InnerAppend: RESULT_INNER::template append<PAIR<TA, TB>> { };
        using type = FoldL_t<B, RESULT_OUTTER, InnerAppend>;
    };

public:
    using type = FoldL_t<A, TypeList<>, OuterAppend>;
};

