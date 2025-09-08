/********n*****************************************************************
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

// 变化1：将 INLOOP 从 bool 改为 size_t，作为“当前匹配深度计数”
template<size_t P = 0, size_t INLOOP = 0, typename ...Cells>
struct Machine {
    using type = Machine<P, INLOOP, Cells...>;

    constexpr static size_t PC = P;
    constexpr static size_t InLoop = INLOOP; // 记录 '[' 与 ']' 的嵌套深度
};

namespace MachineTrait {
    template<typename T, typename U>
    struct Concat;
    template<typename T, typename U>
    using Concat_t = typename Concat<T, U>::type;
    // 注意：Q、R 改为 size_t
    template<size_t P, size_t Q, size_t L, size_t R, typename ...Ts, typename ...Us>
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

    template<size_t PC, size_t INLOOP, typename C, typename... Cells>
    struct IsZero<Machine<PC, INLOOP, C, Cells...>>: IsZero<Machine<PC - 1, INLOOP, Cells...>> {};
    template<size_t INLOOP, typename C, typename... Cells>
    struct IsZero<Machine<0, INLOOP, C, Cells...>>: std::bool_constant<C::value == 0> {}; // IsZero也有value

    template<typename MACHINE>
    struct Inc;
    template<typename MACHINE>
    using Inc_t = typename Inc<MACHINE>::type;
    template<size_t PC, size_t INLOOP, typename C, typename... Cells>
    struct Inc<Machine<PC, INLOOP, C, Cells...>>:
        Concat_t<Machine<PC, INLOOP, C>, Inc_t<Machine<PC - 1, INLOOP, Cells...>>> {};
    template<size_t INLOOP, typename C, typename... Cells>
    struct Inc<Machine<0, INLOOP, C, Cells...>>:
        Machine<0, INLOOP, Cell<C::value + 1>, Cells...> {};

    template<typename MACHINE>
    struct Dec;
    template<typename MACHINE>
    using Dec_t = typename Dec<MACHINE>::type;
    template<size_t PC, size_t INLOOP, typename C, typename... Cells>
    struct Dec<Machine<PC, INLOOP, C, Cells...>>:
        Concat_t<Machine<PC, INLOOP, C>, Dec_t<Machine<PC - 1, INLOOP, Cells...>>> {};
    template<size_t INLOOP, typename C, typename... Cells>
    struct Dec<Machine<0, INLOOP, C, Cells...>>:
        Machine<0, INLOOP, Cell<C::value - 1>, Cells...> {};

    template<typename MACHINE>
    struct Right;
    template<typename MACHINE>
    using Right_t = typename Right<MACHINE>::type;
    template<size_t PC, size_t INLOOP, typename... Cells>
    struct Right<Machine<PC, INLOOP, Cells...>>:
        Machine<PC+1, INLOOP, Cells...> {};

    template<typename MACHINE>
    struct Left;
    template<typename MACHINE>
    using Left_t = typename Left<MACHINE>::type;
    template<size_t PC, size_t INLOOP, typename... Cells>
    struct Left<Machine<PC, INLOOP, Cells...>>:
        Machine<PC-1, INLOOP, Cells...> {};

    // 变化2：Enable/Disable 不再用 bool 开关，而是对“深度计数” +1/-1
    template<typename MACHINE>
    struct EnableLoop;
    template<typename MACHINE>
    using EnableLoop_t = typename EnableLoop<MACHINE>::type;
    template<size_t PC, size_t INLOOP, typename... Cells>
    struct EnableLoop<Machine<PC, INLOOP, Cells...>>:
        Machine<PC, INLOOP + 1, Cells...> {};  // +1

    template<typename MACHINE>
    struct DisableLoop;
    template<typename MACHINE>
    using DisableLoop_t = typename DisableLoop<MACHINE>::type;
    template<size_t PC, size_t INLOOP, typename... Cells>
    struct DisableLoop<Machine<PC, INLOOP, Cells...>>:
        Machine<PC, INLOOP - 1, Cells...> {};  // -1

    template<size_t PC, size_t INLOOP, typename ...Cells>
    inline const auto ToStr(Machine<PC, INLOOP, Cells...>) {
        constexpr static char str[] = { Cells::value ...  };
        return str;
    }
};

// 变化3：BrainFuck 增加参数 CUR（当前欲匹配的 ']' 层号, 即BrainFuck解释器当前执行层），skip 表示是否处于“跳过模式”
template<typename MACHINE, size_t CUR = 0, bool skip = false, char ...cs>
struct BrainFuck: MACHINE {};

template<typename MACHINE, size_t CUR, bool skip, char ...cs>
using BrainFuck_t = typename BrainFuck<MACHINE, CUR, skip, cs...>::type;

template<typename MACHINE, size_t CUR, char c, char ...cs>
struct BrainFuck<MACHINE, CUR, true, c, cs...>
  : BrainFuck_t<MACHINE, CUR, true, cs...> {};

template<typename MACHINE, size_t CUR, bool skip, char c, char ...cs>
struct BrainFuck<MACHINE, CUR, skip, c, cs...>
  : BrainFuck_t<MACHINE, CUR, skip, cs...> {};

template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, false, '+', cs...>
  : BrainFuck_t<MachineTrait::Inc_t<MACHINE>, CUR, false, cs...> {};

template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, false, '-', cs...>
  : BrainFuck_t<MachineTrait::Dec_t<MACHINE>, CUR, false, cs...> {};

template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, false, '<', cs...>
  : BrainFuck_t<MachineTrait::Left_t<MACHINE>, CUR, false, cs...> {};

template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, false, '>', cs...>
  : BrainFuck_t<MachineTrait::Right_t<MACHINE>, CUR, false, cs...> {};

// 变化4：处理 '[' —— 引入 Skip/Enter 两种路径 + Loop 判定，支持多重嵌套与“狂跳”
template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, false, '[', cs...> {
    using EnableLoopedMachine = MachineTrait::EnableLoop_t<MACHINE>;

    // skip 分支：只是“驱动匹配器”在类型层面把对应 ']' 找到；状态对外不改
    template<typename IN>
    struct Skip {
        using __force = BrainFuck_t<IN, CUR, true, cs...>; // 仅触发“狂跳”
        using type = IN;                                   // 保持输入状态
    };
    // enter 分支：进入循环体，深度+1，CUR+1（下一层期望匹配的 ']'）
    template<typename IN>
    struct Enter : BrainFuck_t<MachineTrait::EnableLoop_t<IN>, CUR + 1, false, cs...> {};

    // 首次判断：当前单元为0 → Skip；非0 → Enter
    using Result = typename std::conditional_t<
        MachineTrait::IsZero_t<MACHINE>::value,
        Skip<MACHINE>,
        Enter<MACHINE>
    >::type;

    // 循环是否继续：Result 非零 → 递归回到 '['；为零 → 退出并把之后的 ']' 当作匹配(由 skip 逻辑完成)
    template<typename IN, bool = (! MachineTrait::IsZero_t<IN>::value)> 
    struct Loop;
    template<typename IN> // 继续循环
    struct Loop<IN, true>: BrainFuck_t<IN, CUR, false, '[', cs...> {};
    template<typename IN> // 结束循环：进入“跳过模式”以匹配退出的那一层 ']'
    struct Loop<IN, false>: BrainFuck_t<MachineTrait::EnableLoop_t<IN>, CUR, true, cs...> {};

    using type = typename Loop<Result>::type;
};

// 变化5：在“跳过模式”下遇到 '[' —— 深度+1，继续跳
template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, true, '[', cs...> : BrainFuck_t<MachineTrait::EnableLoop_t<MACHINE>, CUR, true, cs...> {};

// 变化6：正常模式下遇到 ']' —— 仅做深度-1（回到外层）；真正的“是否回跳”由上面的 '[' 分支控制
template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, false, ']', cs...> : MachineTrait::DisableLoop_t<MACHINE> {};

// 变化7：跳过模式下遇到 ']' —— 若当前 ']'' 就是要匹配的那一层(CUR==InLoop)，则退出跳过模式
template<typename MACHINE, size_t CUR, char ...cs>
struct BrainFuck<MACHINE, CUR, true, ']', cs...> {
    using DisableLoopMachine = MachineTrait::DisableLoop_t<MACHINE>;

    template<typename IN, bool = (CUR == IN::InLoop)> // 是否匹配到目标层的 ']'
    struct Select;
    template<typename IN> // 命中匹配层：退出跳过模式，继续正常解析
    struct Select<IN, true>: BrainFuck_t<IN, CUR, false, cs...> {};
    template<typename IN> // 还没到匹配层：继续跳
    struct Select<IN, false>: BrainFuck_t<IN, CUR, true, cs...> {};

    using type = typename Select<DisableLoopMachine>::type;
};

template<typename T, T... cs>
constexpr auto operator ""_brain_fuck() {
    using Machine = MachineTrait::InitMachine_t<30>;          // 变化8：扩展 tape 长度以跑更复杂的嵌套
    using Result = BrainFuck_t<Machine, 0, false, cs...>;     // 从 CUR=0、非跳过 开始

    return MachineTrait::ToStr(Result{});
};

int main(int argc, char** argv) {    
    // 用双重循环打印 "!"
    puts( R"(
        >>++++++[<++[<+++>-]>-]<<---          ; !
    )"_brain_fuck);
    
    
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
    >>>++++++[<++[<+++>-]>-]<<---.            ; !
    )"_brain_fuck );

    return 0;
}
