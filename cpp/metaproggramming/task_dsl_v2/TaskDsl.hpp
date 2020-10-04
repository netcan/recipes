/*************************************************************************
    > File Name: TaskDsl.hpp
    > Author: Netcan
    > Descripton: TaskDsl
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-29 18:28
************************************************************************/
#pragma once
#include "Typelist.hpp"
#include "JobTrait.hpp"
#include "Connection.hpp"
#include "TaskAnalyzer.hpp"
#include "MetaMacro.hpp"

///////////////////////////////////////////////////////////////////////////////
template<typename = void, typename ...Chains>
class TaskDsl {
    using Links = Unique_t<Flatten_t<TypeList<typename Chain<Chains>::type...>>>;
    using Analyzer = typename Links::template exportTo<TaskAnalyzer>;

    using AllJobs = typename Analyzer::AllJobs;
    using JobsCb = typename Map_t<AllJobs, JobCb>::template exportTo<std::tuple>;

    using OneToOneLinkSet = typename Analyzer::OneToOneLinkSet;
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

///////////////////////////////////////////////////////////////////////////////
#define __def_task(name, ...)         \
    struct name: JobSignature {         \
        auto operator()() __VA_ARGS__     \
    }; \

#define __chain(n, link) ,link -> void
#define __some_job(...) auto(*)(SomeJob<__VA_ARGS__>)
#define __fork(...) __some_job(__VA_ARGS__)
#define __merge(...) __some_job(__VA_ARGS__)
#define __job(Job) auto(*)(Job)
#define __taskbuild(...) TaskDsl<void PASTE(REPEAT_, GET_ARG_COUNT(__VA_ARGS__)) (__chain, 0, __VA_ARGS__)>{}
