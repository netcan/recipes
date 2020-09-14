/*************************************************************************
    > File Name: TaskAnalyzer.hpp
    > Author: Netcan
    > Descripton: TaskAnalyzer
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-29 18:27
************************************************************************/
#pragma once
#include "Typelist.hpp"
#include "Connection.hpp"
template<typename ...Links>
class TaskAnalyzer {
    template<typename Link>
    class OneToOneLinkSetF {
        using FromJobList = typename Link::FromJobList;
        using ToJobList = typename Link::ToJobList;
    public:
        using type = CrossProduct_t<FromJobList, ToJobList, OneToOneLink>;
    };

public:
    using AllJobs = Unique_t<Concat_t<typename Links::FromJobList..., typename Links::ToJobList...>>;
    using OneToOneLinkSet = Unique_t<Flatten_t<Map_t<TypeList<Links...>, OneToOneLinkSetF>>>;
};
