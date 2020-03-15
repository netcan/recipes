#include <iostream>
#include <array>

template <size_t N>
using Chars = std::array<char, N>;

template<size_t N, size_t idx = 0, size_t... Is>
constexpr Chars<sizeof...(Is)> fromStr(const char (&str)[N], std::index_sequence<Is...>, std::integral_constant<size_t, idx>) {
    return { str[Is + idx]... };
}

template<size_t idx = 0, size_t Len = 1, size_t N>
constexpr Chars<Len> splitStr(const char (&str)[N]) {
    return fromStr(str, std::make_index_sequence<Len>{}, std::integral_constant<size_t, idx>{});
}

int main(int argc, char** argv) {
    auto result = splitStr<6,5>("Hello world"); // 符号表仅存储`world'
    printf("%s %ld\n", result.data(), result.size());
    return 0;
}
