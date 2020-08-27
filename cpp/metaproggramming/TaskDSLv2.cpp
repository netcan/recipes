/*************************************************************************
    > File Name: TaskDSLv2.cpp
    > Author: Netcan
    > Descripton: TaskDSLv2
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-27 22:38
************************************************************************/
#include <cstdio>
#include <memory>
#include "Typelist.hpp"

template<typename T>
class Connection;

struct JobSignature {};

template<typename J, typename = void>
struct JobTrait;

// a job self
template<typename J>
struct JobTrait<J, std::void_t<std::is_base_of<JobSignature, J>>> {
    using JobList = TypeList<J>;
};

template<typename F, typename T>
class Connection<auto(F) -> T> {
    using FROMs = typename JobTrait<F>::JobList;
    using TOs = typename JobTrait<T>::JobList;
public:
    using FromJobList = Unique_t<Flatten_t<FROMs>>;
    using ToJobList = Unique_t<Flatten_t<TOs>>;
};

struct Job;
template<typename J>
struct JobCb {
    using type = JobCb<J>;
    using JobType = J;
    std::unique_ptr<Job> job_{JobType::Create()};
};

template<typename F, typename T>
struct OneToOneLink {
    using FromJob = F;
    using ToJob = T;
};

template<typename ...Links>
class TaskAnalyzer {
    template<typename FROMs, typename TOs, typename = void>
    struct BuildOneToOneLink;

    template<typename ...Fs, typename Ts>
    struct BuildOneToOneLink<TypeList<Fs...>, Ts> {
        using type = Concat_t<typename BuildOneToOneLink<Fs, Ts>::type...>;
    };

    template<typename F, typename... Ts>
    struct BuildOneToOneLink<F, TypeList<Ts...>, std::enable_if_t<!IsTypeList_v<F>>> {
        using type = TypeList<OneToOneLink<F, Ts>...>;
    };

    template<typename Link>
    class OneToOneLinkSetF{
        using FromJobList = typename Link::FromJobList;
        using ToJobList = typename Link::ToJobList;
    public:
        using type = typename BuildOneToOneLink<FromJobList, ToJobList>::type;
    };

public:
    using AllJobs = Unique_t<Concat_t<typename Connection<Links>::FromJobList..., typename Connection<Links>::ToJobList...>>;
    using OneToOneLinkSet = Unique_t<Map_t<TypeList<Connection<Links>...>, OneToOneLinkSetF>>;
};

template<typename ...Links>
class Task {
    using AllJobs = typename TaskAnalyzer<Links...>::AllJobs;
    using OneToOneLinkSet = typename TaskAnalyzer<Links...>::OneToOneLinkSet;
private:
    using JobsCb = Map_t<AllJobs, JobCb>;
    typename JobsCb::template exportTo<std::tuple> jobsCb_;
};

struct Job {
    Job(const char* name): name_(name) { printf("%s Created\n", name); }
    void precede(const Job& b) { printf("%s -> %s\n", name_, b.name_); }
    ~Job() { printf("%s Destroyed\n", name_); }
private:
    const char* name_;
};

struct A: JobSignature {
    static Job* Create() { return new Job("A"); }
};

struct B: JobSignature {
    static Job* Create() { return new Job("B"); }
};

struct C: JobSignature {
    static Job* Create() { return new Job("C"); }
};

struct D: JobSignature {
    static Job* Create() { return new Job("D"); }
};

int main(int argc, char** argv) {
    Task<auto(A) -> B,
        auto(A) -> C,
        auto(B) -> D,
        auto(C) -> D> t;
    return 0;
}
