/*************************************************************************
    > File Name: TimePerf.hpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: netcan1996@gmail.com
    > Created Time: 2024-09-05 00:11
************************************************************************/

#pragma once
#include <chrono>
namespace utils {
using high_resolution_clock = std::chrono::high_resolution_clock;
struct TimePerf {
    TimePerf(high_resolution_clock::duration &duration) noexcept
        : now_(high_resolution_clock::now()), duration_(duration) {}
    ~TimePerf() { duration_ = high_resolution_clock::now() - now_; }

  private:
    std::chrono::time_point<high_resolution_clock> now_;
    high_resolution_clock::duration& duration_;
};
};
