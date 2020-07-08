#include <iostream>
#include <bitset>

// .template限定
template <size_t N>
void printBitSet(const std::bitset<N>& bs) {
    std::cout << bs.template to_string<char>() << std::endl;
}

template<typename T>
struct Base {
    void exit() {}
};

template<typename T>
struct Deived: Base<T> {
    void foo() {
        // 默认不会访问基类模板成员、函数，用this限定，this->exit()或者Base<T>::exit()
        Base<T>::exit();
    }
};

// 模板的模板参数，class关键字
template<typename T, template <typename> class Cont>
class Stack {
    Cont<T> elems;
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
};

int main(int argc, char** argv) {
    int x = int();
    printf("%d\n", x);
    return 0;
}
