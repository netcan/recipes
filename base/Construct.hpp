/* author: netcan
 * date: 2019/08/14 22:04
 * @shanghai
 */
#pragma once
#include <memory>
#include "RetCode.hpp"

namespace base {
template <typename T>
static auto CallOnConstructed(T* obj) -> decltype(obj->OnConstructed(), RetCode()) {
    return obj->OnConstructed();
}

template <typename T>
static auto CallOnConstructed(...) -> RetCode {
    return OK;
}

template<typename T, typename... Args>
T* Construct(Args&& ...args) noexcept {
    T* obj = new T(std::forward<Args>(args)...);
    if(CallOnConstructed<T>(obj) != OK) {
        // TODO: log it error! OnConstructed failed
        delete obj;
        return nullptr;
    }
    return obj;
}

template<typename T, typename... Args>
std::shared_ptr<T> make_shared_construct(Args&& ...args) noexcept {
    std::shared_ptr<T> spObj =
        std::make_shared<T>(std::forward<Args>(args)...);
    if(CallOnConstructed<T>(spObj.get()) != OK) {
        // TODO: log it error! OnConstructed failed
        return nullptr;
    }
    return std::move(spObj);
}
}
