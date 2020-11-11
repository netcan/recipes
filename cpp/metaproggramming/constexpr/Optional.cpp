/*************************************************************************
    > File Name: Optional.cpp
    > Author: Netcan
    > Descripton: Optional
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-09 22:42
************************************************************************/
#include <type_traits>
#include <string>
#include <thread>

template<typename T>
union Storage {
    char dummy;
    constexpr Storage(): dummy{} {}
    constexpr ~Storage() {}
    T data;
};

///////////////////////////////////////////////////////////////////////////////
struct OptionalTrivially {};

template <typename T, typename Contained>
struct OptionalNonTrivially {
    ~OptionalNonTrivially() {
        if (static_cast<T*>(this)->initialized) {
            static_cast<T*>(this)->storage_.data.~Contained();
        }
    }
};

#if 0
template <typename Contained>
struct Optional: std::conditional_t<std::is_trivially_destructible_v<Contained>,
                                    OptionalTrivially,
                                    OptionalNonTrivially<Optional<Contained>, Contained>> {
    constexpr Optional& operator=(Contained&& data) {
        storage_.data = std::move(data);
        initialized = true;
        return *this;
    }

    Storage<Contained> storage_;
    bool initialized{};
};
#endif

///////////////////////////////////////////////////////////////////////////////
#if 1
template <typename Contained>
struct Optional {
    constexpr Optional& operator=(Contained&& data) {
        storage_.data = std::move(data);
        initialized = true;
        return *this;
    }

    // constexpr ~Optional() requires (! std::is_trivially_destructible_v<Contained>) {
        // if (initialized) {
            // this->storage_.data.~Contained();
        // }
    // }
    // constexpr ~Optional() = default;

    constexpr ~Optional() {
        if constexpr(! std::is_trivially_destructible_v<Contained>) {
            if (initialized) {
                this->storage_.data.~Contained();
            }
        }
    }

    Storage<Contained> storage_;
    bool initialized{};
};
#endif

struct Trivially { };
struct NonTrivially {
    ~NonTrivially() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(3s);
    }
};

int main(int argc, char** argv) {
    {
        puts("ctor Trivially");
        Optional<Trivially> o1;
        o1 = Trivially{};
        puts("dtor Trivially");
    }

    {
        puts("ctor Trivially");
        Optional<NonTrivially> o2;
        o2 = NonTrivially{};
        puts("dtor Trivially");
    }

    return 0;
}
