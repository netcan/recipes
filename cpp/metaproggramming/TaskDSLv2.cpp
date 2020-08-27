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
struct Connection;

struct JobSignature {};

template<typename J, typename = void>
struct JobTrait;

template<typename J>
struct JobTrait<J, std::void_t<std::is_base_of<JobSignature, J>>> {
    using JobType = TypeList<J>;
};

template<typename F, typename T>
struct Connection<auto(F) -> T> {
    using FROM = typename JobTrait<F>::JobType;
    using TO = typename JobTrait<T>::JobType;
};

struct Job;
template<typename J>
struct JobCb {
    using type = JobCb<J>;
    using JobType = J;
    JobCb(): job_(JobType::Create()) {};

private:
    std::unique_ptr<Job> job_;
};

template<typename ...Links>
struct TaskAnalyzer {
    using AllJobs = Unique_t<Concat_t<typename Connection<Links>::FROM..., typename Connection<Links>::TO...>>;
};

template<typename ...Links>
struct Task {
private:
    using AllJobs = typename TaskAnalyzer<Links...>::AllJobs;
    using JobsCb = Map_t<AllJobs, JobCb>;
private:
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
