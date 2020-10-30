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
    class JobDescendantsMap {
        template<typename Con>
        struct GetFrom: Return<typename Con::FROM> { };
        template<typename Con>
        struct GetDst: Return<typename Con::DST> { };

        using Connections = Unique_t<TypeList<Connection<LINKS>...>>; // remove same edge
        using FromJobs = Map_t<Connections, GetFrom>;
        using DstJobs = Map_t<Connections, GetDst>;
        using AllJobs = Unique_t<Concat_t<FromJobs, DstJobs>>;

        template<typename J>
        class FindJobDescendants {
            template<typename C> struct Dependency:
                std::is_same<typename GetFrom<C>::type, J> { };
            using JobDescendants = Filter_t<Connections, Dependency>;
        public:
            struct type {
                using Job = J;
                using Descendants = Map_t<JobDescendants, GetDst>;
            };
        };
    public:
        using type = Map_t<AllJobs, FindJobDescendants>;
    };

    // [<Job, Descendants>]
    using JobDescendantsMap_t = typename JobDescendantsMap::type;

    class SortedJobs {
        template<typename DECENDS>
        class FindAllDescendants {
            template<typename ACC, typename Job>
            struct AppendDes {
                template <typename DEP> struct JDepsCond:
                    std::is_same<typename DEP::Job, Job> {};
                using DepsResult = FindBy_t<JobDescendantsMap_t, JDepsCond>; // 从邻接表查找Job的后继节点列表

                using type = Concat_t<typename ACC::template append<Job>,
                      typename FindAllDescendants<typename DepsResult::Descendants>::type>;
            };
        public:
            using type = FoldL_t<DECENDS, TypeList<>, AppendDes>;
        };

        template<typename DEP>
        struct FindJobAllDescendants {
            struct type {
                using Job = typename DEP::Job;
                using AllDescendants =
                    typename FindAllDescendants<typename DEP::Descendants>::type;
            };
        };

        template<typename DEP> struct GetJob: Return<typename DEP::Job> { };

        using JobAllDescendantsMap = Map_t<JobDescendantsMap_t, FindJobAllDescendants>;

        template<typename LHS, typename RHS>
        struct JobCmp: Elem<typename LHS::AllDescendants, typename RHS::Job> { };
    public:
        using type = Map_t<typename Sort<JobAllDescendantsMap, JobCmp>::type, GetJob>;
    };

    template<typename ...Jobs>
    struct Runable { };

    template<auto ...J>
    struct Runable<JobWrapper<J>...> {
        static constexpr void Run() {
            return (JobWrapper<J>{}(),...);
        }
    };

public:
    constexpr void Run() {
        using Jobs = typename SortedJobs::type::template exportTo<Runable>;
        return Jobs::Run();
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
