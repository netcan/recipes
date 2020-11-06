/*************************************************************************
    > File Name: BrainFuckConstexpr.cpp
    > Author: Netcan
    > Descripton: BrainFuck
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-11-06 21:02
************************************************************************/
#include <cstddef>
#include <cstdio>

template<size_t N>
class Stream {
public:
    constexpr void push(char c) { data_[idx_++] = c; }
    constexpr operator const char*() const { return data_; }
    constexpr size_t size() { return idx_; }
private:
    size_t idx_{};
    char data_[N]{};
};

///////////////////////////////////////////////////////////////////////////////
constexpr size_t CELL_SIZE = 16;
template<typename STREAM>
constexpr auto parse(const char* input, bool skip, char* cells,
        size_t& pc, STREAM&& output) -> size_t {
    const char* c = input;
    while(*c) {
        switch(*c) {
            case '+': if (!skip) ++cells[pc]; break;
            case '-': if (!skip) --cells[pc]; break;
            case '.': if (!skip) output.push(cells[pc]); break;
            case '>': if (!skip) ++pc; break;
            case '<': if (!skip) --pc; break;
            case '[': {
                while (!skip && cells[pc] != 0)
                    parse(c + 1, false, cells, pc, output);
                c += parse(c + 1, true, cells, pc, output) + 1;
            } break;
            case ']': return c - input;
            default: break;
        }
        ++c;
    }
    return c - input;
}

template<typename STREAM>
constexpr auto parse(const char* input, STREAM&& output) -> STREAM&& {
    char cells[CELL_SIZE]{};
    size_t pc{};
    parse(input, false, cells, pc, output);
    return output;
}

template<size_t OUTPUT_SIZE = 15>
constexpr auto brain_fuck(const char* input) {
    Stream<OUTPUT_SIZE> output;
    return parse(input, output);
}

// calculate output size
constexpr auto brain_fuck_output_size(const char* input) -> size_t {
    struct {
        size_t sz{};
        constexpr void push(...) { ++sz; }
    } dummy;
    return parse(input, dummy).sz + 1; // include '\0'
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
    // compile time
    constexpr auto res = brain_fuck(R"(
        ++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.
        >---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
    )");
    puts(res);

    // runtime
    if (argc > 1) puts(brain_fuck(argv[1]));

    return 0;
}
