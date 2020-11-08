/*************************************************************************
    > File Name: BrainFuck.cpp
    > Author: Netcan
    > Descripton: BrainFuck
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-03 22:20
************************************************************************/
#include <cstdio>
#include <type_traits>

template<char c>
using Cell = std::integral_constant<char, c>;

template<size_t P = 0, bool INLOOP = false, typename ...Cells>
struct Machine {
    using type = Machine<P, INLOOP, Cells...>;

    constexpr static size_t PC = P;
    constexpr static size_t InLoop = INLOOP;
};

namespace MachineTrait {
    template<typename T, typename U>
    struct Concat;
    template<typename T, typename U>
    using Concat_t = typename Concat<T, U>::type;
    template<size_t P, bool Q, size_t L, bool R, typename ...Ts, typename ...Us>
    struct Concat<Machine<P, Q, Ts...>, Machine<L, R, Us...>>: Machine<P, Q, Ts..., Us...> {};

    template<size_t N>
    struct InitMachine: Concat_t<Machine<0, 0, Cell<0>>, typename InitMachine<N-1>::type> {};
    template<size_t N>
    using InitMachine_t = typename InitMachine<N>::type;
    template<>
    struct InitMachine<0>: Machine<0, 0, Cell<0>> {};

    template<typename MACHINE>
    struct IsZero;
    template<typename MACHINE>
    using IsZero_t = typename IsZero<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename C, typename... Cells>
    struct IsZero<Machine<PC, INLOOP, C, Cells...>>: IsZero<Machine<PC - 1, INLOOP, Cells...>> {};
    template<bool INLOOP, typename C, typename... Cells>
    struct IsZero<Machine<0, INLOOP, C, Cells...>>: std::bool_constant<C::value == 0> {};

    template<typename MACHINE>
    struct Inc;
    template<typename MACHINE>
    using Inc_t = typename Inc<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename C, typename... Cells>
    struct Inc<Machine<PC, INLOOP, C, Cells...>>:
        Concat_t<Machine<PC, INLOOP, C>, Inc_t<Machine<PC - 1, INLOOP, Cells...>>> {};
    template<bool INLOOP, typename C, typename... Cells>
    struct Inc<Machine<0, INLOOP, C, Cells...>>:
        Machine<0, INLOOP, Cell<C::value + 1>, Cells...> {};

    template<typename MACHINE>
    struct Dec;
    template<typename MACHINE>
    using Dec_t = typename Dec<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename C, typename... Cells>
    struct Dec<Machine<PC, INLOOP, C, Cells...>>:
        Concat_t<Machine<PC, INLOOP, C>, Dec_t<Machine<PC - 1, INLOOP, Cells...>>> {};
    template<bool INLOOP, typename C, typename... Cells>
    struct Dec<Machine<0, INLOOP, C, Cells...>>:
        Machine<0, INLOOP, Cell<C::value - 1>, Cells...> {};

    template<typename MACHINE>
    struct Right;
    template<typename MACHINE>
    using Right_t = typename Right<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename... Cells>
    struct Right<Machine<PC, INLOOP, Cells...>>:
        Machine<PC+1, INLOOP, Cells...> {};

    template<typename MACHINE>
    struct Left;
    template<typename MACHINE>
    using Left_t = typename Left<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename... Cells>
    struct Left<Machine<PC, INLOOP, Cells...>>:
        Machine<PC-1, INLOOP, Cells...> {};

    template<typename MACHINE>
    struct EnableLoop;
    template<typename MACHINE>
    using EnableLoop_t = typename EnableLoop<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename... Cells>
    struct EnableLoop<Machine<PC, INLOOP, Cells...>>:
        Machine<PC, true, Cells...> {};

    template<typename MACHINE>
    struct DisableLoop;
    template<typename MACHINE>
    using DisableLoop_t = typename DisableLoop<MACHINE>::type;
    template<size_t PC, bool INLOOP, typename... Cells>
    struct DisableLoop<Machine<PC, INLOOP, Cells...>>:
        Machine<PC, false, Cells...> {};

    template<size_t PC, bool INLOOP, typename ...Cells>
    inline const auto ToStr(Machine<PC, INLOOP, Cells...>) {
        constexpr const static char str[] = { Cells::value ...  };
        return str;
    }
};

template<typename MACHINE, bool skip, char ...cs>
struct BrainFuck: MACHINE {};

template<typename MACHINE, bool skip, char ...cs>
using BrainFuck_t = typename BrainFuck<MACHINE, skip, cs...>::type;

template<typename MACHINE, char c, char ...cs>
struct BrainFuck<MACHINE, true, c, cs...>: BrainFuck_t<MACHINE, true, cs...> {};

template<typename MACHINE, bool skip, char c, char ...cs>
struct BrainFuck<MACHINE, skip, c, cs...>: BrainFuck_t<MACHINE, skip, cs...> {};

template<typename MACHINE, char ...cs>
struct BrainFuck<MACHINE, false, '+', cs...>:
    BrainFuck_t<MachineTrait::Inc_t<MACHINE>, false, cs...> {};

template<typename MACHINE, char ...cs>
struct BrainFuck<MACHINE, false, '-', cs...>:
    BrainFuck_t<MachineTrait::Dec_t<MACHINE>, false, cs...> {};

template<typename MACHINE, char ...cs>
struct BrainFuck<MACHINE, false, '<', cs...>:
    BrainFuck_t<MachineTrait::Left_t<MACHINE>, false, cs...> {};

template<typename MACHINE, char ...cs>
struct BrainFuck<MACHINE, false, '>', cs...>:
    BrainFuck_t<MachineTrait::Right_t<MACHINE>, false, cs...> {};

template<typename MACHINE, char ...cs>
struct BrainFuck<MACHINE, false, '[', cs...> {
    using EnableLoopedMachine = MachineTrait::EnableLoop_t<MACHINE>;

    template<typename IN, bool = MachineTrait::IsZero_t<IN>::value>
    struct Select: BrainFuck_t<IN, true, cs...> {}; // skip
    template<typename IN> // loop
    struct Select<IN, false>: BrainFuck_t<IN, false, cs...> {};

    using Result = typename Select<EnableLoopedMachine>::type;

    template<typename IN, bool =
        (! MachineTrait::IsZero_t<IN>::value && IN::InLoop)>
    struct Loop: IN {};   // skip
    template<typename IN> // continue
    struct Loop<IN, true>: BrainFuck_t<IN, false, '[', cs...> {};

    using type = typename Loop<Result>::type;
};

template<typename MACHINE, char ...cs>
struct BrainFuck<MACHINE, false, ']', cs...> {
    using DisableLoopMachine = MachineTrait::DisableLoop_t<MACHINE>;

    template<typename IN, bool = MachineTrait::IsZero_t<IN>::value>
    struct Select: MACHINE {}; // goback
    template<typename IN>      // skip
    struct Select<IN, true>: BrainFuck_t<DisableLoopMachine, false, cs...> {};

    using type = typename Select<MACHINE>::type;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"
template<typename T, T... cs>
constexpr auto operator ""_brain_fuck() {
    using Machine = MachineTrait::InitMachine_t<15>;
    using Result = BrainFuck_t<Machine, false, cs...>;

    return MachineTrait::ToStr(Result{});
};
#pragma GCC diagnostic pop

int main(int argc, char** argv) {
    puts( R"(
        >++++++++[<+++++++++>-]<.                 ; H
        >>++++++++++[<++++++++++>-]<+.            ; e
        >>+++++++++[<++++++++++++>-]<.            ; l
        >>+++++++++[<++++++++++++>-]<.            ; l
        >>++++++++++[<+++++++++++>-]<+.           ; o
        >>++++[<++++++++>-]<.                     ;
        >>+++++++++++[<++++++++>-]<-.             ; W
        >>++++++++++[<+++++++++++>-]<+.           ; o
        >>++++++++++[<++++++++++++>-]<------.     ; r
        >>+++++++++[<++++++++++++>-]<.            ; l
        >>++++++++++[<++++++++++>-]<.             ; d
        >>++++++[<++++++>-]<---.                  ; !
    )"_brain_fuck );

    return 0;
}
