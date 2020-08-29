/*************************************************************************
    > File Name: JobTrait.hpp
    > Author: Netcan
    > Descripton: JobTrait
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-29 18:22
************************************************************************/
#pragma once
#include "Typelist.hpp"

struct Job {
    Job() = default;
    Job(const char* name): name_(name) { printf("%s Created\n", name); }
    void precede(const Job& b) { printf("%s -> %s\n", name_, b.name_); }
    ~Job() { printf("%s Destroyed\n", name_); }
private:
    const char* name_{};
};

template<typename J>
struct JobCb {
    using type = JobCb<J>;
    using JobType = J;
    Job job_{JobType{}()};
};

template<typename J, typename = void>
struct JobTrait;

struct JobSignature {};
template<typename ...J>
struct SomeJob
{ using JobList = TypeList<J...>; };

// a job self
template<typename J>
struct JobTrait<J, std::void_t<std::is_base_of<JobSignature, J>>> {
    using JobList = TypeList<J>;
};

template<typename ...J>
struct JobTrait<SomeJob<J...>> {
    using JobList = typename SomeJob<J...>::JobList;
};
