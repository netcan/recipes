/*************************************************************************
    > File Name: NthPrime.cpp
    > Author: Netcan
    > Descripton: meta programming for nth prime solver
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-12-28 21:26
************************************************************************/
#include <type_traits>
#include <cstdio>
// Lazy evaluation and infinite streams
// Ref: https://mitpress.mit.edu/sites/default/files/sicp/full-text/book/book-Z-H-24.html#%_sec_3.5.2

template<typename F, typename = void>
struct Eval: F {};

template<typename F>
struct Eval<F, std::void_t<typename F::result>>:
        Eval<typename F::result> {};

/* (cons-stream ⟨a⟩ ⟨b⟩) is equivalent to
   (cons ⟨a⟩ (delay ⟨b⟩)) */
template<typename H, typename L>
struct CONS_STREAM {
    using CAR = H;
    using CDR = Eval<L>; // L is lazy object
};

template<int V>
using Int = std::integral_constant<int, V>;

template<int N> struct IntegersStartingFrom               // (define (integers-starting-from n)
    : CONS_STREAM<Int<N>, IntegersStartingFrom<N+1>> {};  //  (cons-stream n (integers-starting-from (+ n 1))))

/* (define (stream-filter pred stream)
    (if (pred (stream-car stream))
            (cons-stream (stream-car stream) (stream-filter pred (stream-cdr stream)))
            (stream-filter pred (stream-cdr stream)))) */
template<template<typename> typename PRED, typename STREAM> struct STREAM_FILTER
    : std::conditional_t<PRED<typename STREAM::CAR>::value,
                        CONS_STREAM<typename STREAM::CAR, STREAM_FILTER<PRED, typename STREAM::CDR>>,
                        STREAM_FILTER<PRED, typename STREAM::CDR>> { };

template<typename STREAM, size_t N> struct STREAM_REF                // (define (stream-ref s n)
    : std::conditional_t<N == 0,                                     //     (if (= n 0)
                       typename STREAM::CAR,                         //         (stream-car s)
                       STREAM_REF<typename STREAM::CDR, N - 1>> {};  //         (stream-ref (stream-cdr s) (- n 1))))

// (define (divisible? x y) (= (remainder x y) 0))
template<typename X, typename Y> using IsDivisible
    = std::bool_constant<X::value % Y::value == 0>;

template<typename F> using Not
    = std::bool_constant<! F::value>;

///////////////////////////////////////////////////////////////////////////////
// Sieve method
/* (define (sieve stream)
    (cons-stream (stream-car stream)
                 (sieve (stream-filter
                         (lambda (x)
                             (not (divisible?
                                 x (stream-car stream))))
                         (stream-cdr stream))))) */
template<typename STREAM> class Sieve {
    template<typename X>
    using Lambda = Not<IsDivisible<X, typename STREAM::CAR>>;
public:
    using result = CONS_STREAM<typename STREAM::CAR,
                                Sieve<STREAM_FILTER<Lambda, typename STREAM::CDR>>>;
};
///////////////////////////////////////////////////////////////////////////////
// Defining infinite streams implicitly(recursively)
template<typename> struct IsPrime;
using Primes                                            // (define primes
    = CONS_STREAM<Int<2>, STREAM_FILTER<                //  (cons-stream 2 (stream-filter
                            IsPrime,                    //                  prime?
                            IntegersStartingFrom<3>>>;  //                  (integers-starting-from 3))))

template<typename N> struct IsPrime {                                                          // (define (prime? n)
    template<typename PS> struct Iter:                                                         //  (define (iter ps)
        std::conditional_t<(PS::CAR::value * PS::CAR::value > N::value), std::true_type,       //      (cond ((> (square (stream-car ps)) n) true)
                            std::conditional_t<IsDivisible<N, typename PS::CAR>::value,        //          ((divisible? n (stream-car ps)) false)
                                               std::false_type, Iter<typename PS::CDR> > > {}; //          (else (iter (stream-cdr ps)))))
    constexpr static bool value = Iter<Primes>::value;                                         //  (iter primes))
};

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    {
        /* (define primes (sieve (integers-starting-from 2))) */
        using Primes = Eval<Sieve<IntegersStartingFrom<2>>>;
        printf("100th prime: %d\n", STREAM_REF<Primes, 100>::value);
    }

    {
        printf("100th prime: %d\n", STREAM_REF<::Primes, 100>::value);
    }

    return 0;
}
