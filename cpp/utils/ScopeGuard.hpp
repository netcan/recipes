/*************************************************************************
    > File Name: ScopeGuard.hpp
    > Author: Netcan
    > Descripton: ScopeGuard for exit handler
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-04-21 23:34
************************************************************************/
#ifndef __SCOPE_GUARD_H__
#define __SCOPE_GUARD_H__
#include <memory>

namespace utils {
template<typename Fn>
class ScopeGuard {
public:
    ScopeGuard(Fn&& fn): fn_(std::forward<Fn>(fn)), enable_(true) {};
    ~ScopeGuard() { if (enable_) fn_(); }
    void dismiss() { enable_ = false; }

private:
    Fn fn_;
    bool enable_;
};

namespace detail {
struct ScopeGuardOnExit {};
template<typename Fn>
static inline ScopeGuard<Fn> operator+(ScopeGuardOnExit, Fn&& fn)
{ return ScopeGuard<Fn>(std::forward<Fn>(fn)); }
};

#define ON_SCOPE_EXIT \
    auto __exit_guard__ = base::detail::ScopeGuardOnExit{} + [&]
#define CANCEL_SCOPE_EXIT \
    __exit_guard__.dismiss()
}

#endif
