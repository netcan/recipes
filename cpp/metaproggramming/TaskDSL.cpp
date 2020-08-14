/*************************************************************************
    > File Name: TaskDSL.cpp
    > Author: Netcan
    > Descripton: TaskDSL
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-14 20:57
************************************************************************/
#include <tuple>
#include "Typelist.hpp"

template<typename...> struct dump;

template<typename T>
struct Connection {};

template<typename Job1, typename Job2>
struct Connection<auto(Job1) -> Job2> {
    using FROM = Job1;
    using DST = Job2;
};

template <typename... LINKS>
struct Task {
    template<typename T>
    struct GetFrom { using type = typename T::FROM; };
    template<typename T>
    struct GetDst { using type = typename T::DST; };

    using Connections = TypeList<Connection<LINKS>...>;
    using FromJobs = typename Map<Connections, GetFrom>::type;
    using Jobs = typename Map<Connections, GetDst, FromJobs>::type;
    using AllJobs = typename Unique<Jobs>::type;
    static constexpr size_t JobNums = AllJobs::size;
    static_assert(JobNums == 3, "");

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
            using Node = typename DEP::head;
            using Dependencies = typename DEP::tails;
        };
    };

    using AllDependency = typename Map<
            typename Map<AllJobs, FindJobDependencies>::type,
            Dependency>::type;

    // dump<typename AllJobs::head> d;
    // dump<AllDependency> dd;

    // template<typename J, typename D>
    // struct Adjacency {
        // using type = std::pair<J, TypeList<D>>;
    // };



    // dump<typename Unique<TypeList<int, int, int>>::type> d;

};

template<typename J>
struct Job {
    using type = J;
};

#define __task(...) Task<__VA_ARGS__>
#define __src(job) auto (Job<job>)
#define __dst(job) Job<job>


struct Job0 {};
struct Job1 {};
struct Job2 {};

using tasks = __task(
        __src(Job0) -> __dst(Job1),
        __src(Job1) -> __dst(Job2));

int main(int argc, char** argv) {
    tasks ts;

    return 0;
}
