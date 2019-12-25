/*************************************************************************
    > File Name: ADL.cpp
    > Author: Netcan
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2019-12-25 22:07
************************************************************************/

#include <iostream>

namespace X {
    template <typename T> void f(T) {
        std::cout << "X::f(T)" << std::endl;
    }
}

namespace N {
    using namespace X; // ADL ignore using-directive
    enum E { e1 };
    void f(E) {
        std::cout << "N::f(T)" << std::endl;
    }
}

void f(int) {
    std::cout << "::f(T)" << std::endl;
}

int main(int argc, char** argv) {
    ::f(N::e1); // ::f(T)
    f(N::e1); // N::f(T), if N is a class, result is ::f(T)
    return 0;
}
