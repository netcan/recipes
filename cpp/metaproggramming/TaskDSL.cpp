/*************************************************************************
    > File Name: TaskDSL.cpp
    > Author: Netcan
    > Descripton: TaskDSL
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-14 20:57
************************************************************************/
#include <tuple>
#include <cstdio>
#include "Typelist.hpp"

//////////////////////////////////////////////////////////////////
// TaskDSL Implementation
template<auto J>
struct JobWrapper {
    void operator()() {
        return J();
    }
};

template<typename LINK>
struct Connection {};

template<typename Job1, typename Job2>
struct Connection<auto(*)(Job1) -> auto(*)(Job2) -> void> {
    using FROM = Job1;
    using DST = Job2;
};

template <typename... LINKS>
class Task {
    template<typename Con>
    struct GetFrom { using type = typename Con::FROM; };
    template<typename Con>
    struct GetDst { using type = typename Con::DST; };

    using Connections = TypeList<Connection<LINKS>...>;
    using FromJobs = typename Map<Connections, GetFrom>::type;
    using Jobs = typename Map<Connections, GetDst, FromJobs>::type;
    using AllJobs = typename Unique<Jobs>::type;
    static constexpr size_t JobNums = AllJobs::size;

    template<typename Job>
    class FindJobDependencies {
        template<typename C> struct Dependency
        { static constexpr bool value = std::is_same_v<typename GetFrom<C>::type, Job>; };
        using JobDependencies = typename Filter<Connections, Dependency>::type;
    public:
        using type = typename Map<JobDependencies, GetDst>::type::template prepend<Job>; // prepend FROM node to result front
    };

    template <typename DEP>
    struct Dependency {
        struct type {
            using Job = typename DEP::head;
            using Dependencies = typename DEP::tails;
        };
    };
    using JobDependenciesMap = typename Map<
            typename Map<AllJobs, FindJobDependencies>::type, // Dst Job List of List
            Dependency>::type;

    template<typename DEPS, typename OUT = TypeList<>, typename = void>
    struct FindAllDependencies {
        using type = OUT; // 边界case，结束递归时直接输出结果
    };
    template<typename DEPS, typename OUT>
    class FindAllDependencies<DEPS, OUT, std::void_t<typename DEPS::head>> {
        using J = typename DEPS::head; // 取出后继表中第一个Job
        template <typename DEP> struct JDepsCond
        { static constexpr bool value = std::is_same_v<typename DEP::Job, J>; };
        using DepsResult = typename FindBy<JobDependenciesMap, JDepsCond>::type; // 从邻接表查找Job的后继节点列表
            using JDeps = typename Unique<typename DepsResult::Dependencies>::type; // 去重操作
    public:
        using type = typename FindAllDependencies<
            typename JDeps::template exportTo<DEPS::tails::template append>::type,
            typename OUT::template appendTo<J>>::type; // 将得到的后继列表合并，进一步递归展开，并输出当前Job到列表
    };

    template<typename DEP>
    struct FindJobAllDependencies {
        struct type {
            using Job = typename DEP::Job;
            using AllDependencies = typename FindAllDependencies<typename DEP::Dependencies>::type;
        };
    };
    template<typename DEP>
    struct GetJob { using type = typename DEP::Job; };
    using JobAllDependenciesMap = typename Map<JobDependenciesMap, FindJobAllDependencies>::type;

    template<typename LHS, typename RHS>
    struct JobCmp {
        static constexpr bool value =
            Elem<typename LHS::AllDependencies, typename RHS::Job>::value;
    };
    using SortedJobs = typename Map<typename Sort<JobAllDependenciesMap, JobCmp>::type, GetJob>::type;

    template<typename ...Jobs>
    struct Runable { };
    template<auto ...J>
    struct Runable<JobWrapper<J>...> {
        void operator()() {
            return (JobWrapper<J>{}(),...);
        }
    };

public:
    void Run() {
        using Jobs = typename SortedJobs::template exportTo<Runable>;
        return Jobs{}();
    }
};

#define __task(...) Task<__VA_ARGS__>{}
#define __job(job) auto(*) (JobWrapper<job>)
#define __link(link) link -> void

//////////////////////////////////////////////////////////////////
// TaskDSL Usecase

void Meat()
{ puts("肉..."); }
void Garnish()
{ puts("配菜..."); }
void Plating()
{ puts("装配..."); }
void Servce1()
{ puts("送菜1..."); }
void Servce2()
{ puts("送菜2..."); }

int main(int argc, char** argv) {
    __task(
        __link(__job(Plating) -> __job(Servce1)),
        __link(__job(Meat)    -> __job(Plating)),
        __link(__job(Plating) -> __job(Servce2)),
        __link(__job(Garnish) -> __job(Plating))).Run();

    return 0;
}
