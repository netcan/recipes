/*************************************************************************
    > File Name: Connection.hpp
    > Author: Netcan
    > Descripton: Connection
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-29 18:24
************************************************************************/
#pragma once
#include "Typelist.hpp"
#include "TupleUtils.hpp"
#include "JobTrait.hpp"
#include <tuple>

template<typename F, typename T>
class Connection {
    using FROMs = typename JobTrait<F>::JobList;
    using TOs = typename JobTrait<T>::JobList;
public:
    using FromJobList = Unique_t<Flatten_t<FROMs>>;
    using ToJobList = Unique_t<Flatten_t<TOs>>;
};

template<typename T, typename OUT = TypeList<>>
struct Chain;

template<typename F, typename OUT>
struct Chain<auto(*)(F) -> void, OUT> {
    using From = F;
    using type = OUT;
};

template<typename F, typename T, typename OUT>
struct Chain<auto(*)(F) -> T, OUT> {
private:
    using To = typename Chain<T, OUT>::From;
public:
    using From = F;
    using type = typename Chain<T, typename OUT::template appendTo<Connection<From, To>>>::type;
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
