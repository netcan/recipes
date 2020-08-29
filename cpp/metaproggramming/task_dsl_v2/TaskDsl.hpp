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
#include "TupleUtils.hpp"
#include "Connection.hpp"
#include "TaskAnalyzer.hpp"
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
#define __def_task(name, ...)         \
  struct name: JobSignature {         \
    auto operator()() __VA_ARGS__     \
  }
