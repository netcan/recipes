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
#include "TupleUtils.hpp"

struct Job {
    Job(const char* name): name_(name) { printf("%s Created\n", name); }
    void precede(const Job& b) { printf("%s -> %s\n", name_, b.name_); }
    ~Job() { printf("%s Destroyed\n", name_); }
private:
    const char* name_;
};

template<typename T>
class Connection;

struct JobSignature {};
template<typename ...J>
struct SomeJob
{ using JobList = TypeList<J...>; };

template<typename J, typename = void>
struct JobTrait;

// a job self
template<typename J>
struct JobTrait<J, std::void_t<std::is_base_of<JobSignature, J>>> {
    using JobList = TypeList<J>;
};

template<typename ...J>
struct JobTrait<SomeJob<J...>> {
    using JobList = typename SomeJob<J...>::JobList;
};

template<typename F, typename T>
class Connection<auto(F) -> T> {
    using FROMs = typename JobTrait<F>::JobList;
    using TOs = typename JobTrait<T>::JobList;
public:
    using FromJobList = Unique_t<Flatten_t<FROMs>>;
    using ToJobList = Unique_t<Flatten_t<TOs>>;
};

template<typename J>
struct JobCb {
    using type = JobCb<J>;
    using JobType = J;
    Job job_{JobType::Create()};
};

template<typename FROM, typename TO>
struct OneToOneLink {
    template<typename JobsCB>
    class InstanceType {
        template<typename J>
        struct IsJob {
            template<typename JobCb> struct apply
            { constexpr static bool value = std::is_same_v<typename JobCb::JobType, J>; };
        };
    public:
        constexpr void build(JobsCB& jobsCb) {
            constexpr size_t JobsCBSize = std::tuple_size_v<JobsCB>;
            constexpr size_t FromJobIndex = TupleElementByF_v<JobsCB, IsJob<FROM>::template apply>;
            constexpr size_t ToJobIndex = TupleElementByF_v<JobsCB, IsJob<TO>::template apply>;
            static_assert(FromJobIndex < JobsCBSize && ToJobIndex < JobsCBSize, "fatal: not find JobCb in JobsCB");
            std::get<FromJobIndex>(jobsCb).job_.precede(std::get<ToJobIndex>(jobsCb).job_);
        }
    };
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
    class OneToOneLinkSetF {
        using FromJobList = typename Link::FromJobList;
        using ToJobList = typename Link::ToJobList;
    public:
        using type = typename BuildOneToOneLink<FromJobList, ToJobList>::type;
    };

public:
    using AllJobs = Unique_t<Concat_t<typename Connection<Links>::FromJobList..., typename Connection<Links>::ToJobList...>>;
    using OneToOneLinkSet = Unique_t<Flatten_t<Map_t<TypeList<Connection<Links>...>, OneToOneLinkSetF>>>;
};

template<typename ...Links>
class TaskDsl {
    using AllJobs = typename TaskAnalyzer<Links...>::AllJobs;
    using JobsCb = typename Map_t<AllJobs, JobCb>::template exportTo<std::tuple>;

    using OneToOneLinkSet = typename TaskAnalyzer<Links...>::OneToOneLinkSet;
    template <typename OneToOneLink>
    struct OneToOneLinkInstanceType
    { using type = typename OneToOneLink::template InstanceType<JobsCb>; };
    using OneToOneLinkInstances =
        typename Map_t<OneToOneLinkSet, OneToOneLinkInstanceType>::template exportTo<std::tuple>;
public:
    constexpr TaskDsl() {
        build(std::make_index_sequence<OneToOneLinkSet::size>{});
    }
private:
    template<size_t ...Is>
    void build(std::index_sequence<Is...>) {
        (std::get<Is>(links_).build(jobsCb_), ...);
    }
private:
    JobsCb jobsCb_;
    OneToOneLinkInstances links_;
};

#define __some_job(...) SomeJob<__VA_ARGS__>
#define __fork(...) __some_job(__VA_ARGS__)
#define __merge(...) auto(__some_job(__VA_ARGS__))
#define __link(Job) auto(Job)
#define __taskbuild(...) TaskDsl<__VA_ARGS__>{}

struct A: JobSignature {
    static Job Create() { return Job("A"); }
};

struct B: JobSignature {
    static Job Create() { return Job("B"); }
};

struct C: JobSignature {
    static Job Create() { return Job("C"); }
};

struct D: JobSignature {
    static Job Create() { return Job("D"); }
};

int main(int argc, char** argv) {
    __taskbuild(
        __link(A) -> __fork(C, D),
        __merge(C, B) -> A
    );

    return 0;
}
