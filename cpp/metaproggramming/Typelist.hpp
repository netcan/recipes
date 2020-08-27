/*************************************************************************
    > File Name: Typelist.hpp
    > Author: Netcan
    > Descripton: Typelist for meta programming
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-26 21:10
************************************************************************/
#include <type_traits>
#include <cstddef>

template<typename...>
struct dump;

template <typename ...Ts>
struct TypeList {
    using type = TypeList<Ts...>;
    static constexpr size_t size = 0;

    template <typename ...T>
    struct append {
        using type = TypeList<T...>;
    };
    template <typename ...T>
    using appendTo = typename append<T...>::type;

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
    struct append {
        using type = TypeList<Head, Tails..., Ts...>;
    };
    template <typename ...Ts>
    using appendTo = typename append<Ts...>::type;

    template <typename T>
    using prepend = typename TypeList<T, Head, Tails...>::type;

    template <template<typename...> typename T>
    using exportTo = T<Head, Tails...>;
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
    using satisfied = typename S::template appendTo<typename IN::head>::type;
public:
    using type = typename Partition<typename IN::tails, P, satisfied, R>::type;
};

template<typename IN,
    template<typename> typename P,
    typename S,
    typename R>
class Partition<IN, P, S, R, std::enable_if_t<!P<typename IN::head>::value> >  {
    using rest = typename R::template appendTo<typename IN::head>::type;
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

template<typename IN, typename OUT = TypeList<>, typename = void>
struct Flatten {
    using type = OUT;
};

template<typename IN>
using Flatten_t = typename Flatten<IN>::type;

template<typename IN, typename OUT>
struct Flatten<IN, OUT, std::enable_if_t<IsTypeList_v<typename IN::head>>> {
    using type = typename Flatten<typename IN::tails, Concat_t<OUT, Flatten_t<typename IN::head>>>::type;
};

template<typename IN, typename OUT>
struct Flatten<IN, OUT, std::enable_if_t<! IsTypeList_v<typename IN::head>>> {
    using type = typename Flatten<typename IN::tails, typename OUT::template appendTo<typename IN::head>>::type;
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
    using type = Concat_t<typename SmallerSorted::template appendTo<H>, BiggerSorted>;
};

template<typename IN, template <typename> class F>
struct Map {
    using type = TypeList<>;
};

template<typename IN, template <typename> class F>
using Map_t = typename Map<IN, F>::type;

template<template <typename> class F, typename ...Ts>
struct Map<TypeList<Ts...>, F> {
    using type = TypeList<typename F<Ts>::type...>;
};

template<typename IN, template <typename> class F, typename OUT = TypeList<>, typename = void>
struct Filter {
    using type = OUT;
};

template<typename IN, template <typename> class F>
using Filter_t = typename Filter<IN, F>::type;

template<typename IN, template <typename> class F, typename OUT>
class Filter<IN, F, OUT, std::void_t<typename IN::head>> {
    using H = typename IN::head;
public:
    using type = typename std::conditional_t<F<H>::value,
          Filter<typename IN::tails, F, typename OUT::template appendTo<H>>,
          Filter<typename IN::tails, F, OUT>>::type;
};


template<typename IN, typename = void>
struct Unique {
    using type = IN;
};

template<typename IN>
using Unique_t = typename Unique<IN>::type;

template<typename IN>
class Unique<IN, std::void_t<typename IN::head>> {
    template<typename T>
    struct IsDifferR {
        template<typename R>
        struct apply { static constexpr bool value = !std::is_same_v<T, R>; };
    };

    using tails = Unique_t<typename IN::tails>;
    using eraseHead = Filter_t<tails, IsDifferR<typename IN::head>::template apply>;
public:
    using type = typename eraseHead::template prepend<typename IN::head>;
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

struct Nil;
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
