/*************************************************************************
    > File Name: Calendar.cpp
    > Author: Netcan
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2021-07-25 11:47
************************************************************************/
// g++-11 -std=c++2a Calendar.cpp
#include <ranges>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <variant>
#include <string>

namespace ranges = std::ranges;
namespace views = std::views;
namespace chrono = std::chrono;
///////////////////////////////////////////////////////////////////////////////
// Date
struct Date {
    using difference_type = std::ptrdiff_t;
    Date() = default;
    Date(uint16_t year, uint16_t month, uint16_t day):
        days_{ chrono::year(year) / chrono::month(month) / chrono::day(day) } { }

    friend bool operator==(const Date&, const Date&) = default;
    Date& operator++()            { ++days_; return *this;                                               }
    Date operator++(int)          { Date tmp(*this); ++*this; return tmp;                                }
    uint16_t day() const          { return static_cast<unsigned>(chrono::year_month_day(days_).day());   }
    uint16_t month() const        { return static_cast<unsigned>(chrono::year_month_day(days_).month()); }
    const char* monthName() const { return MONTH_NAME[month()];                                          }
    uint16_t year() const         { return static_cast<int>(chrono::year_month_day(days_).year());       }
    bool weekdaylessThan(const Date& rhs) {
        return chrono::weekday(days_).c_encoding() <
            chrono::weekday(rhs.days_).c_encoding();
    }
    uint16_t dayOfWeek() const {
        return chrono::weekday(days_).c_encoding();
    }

private:
    static constexpr const char* MONTH_NAME[] = {
        "", "January", "February", "March",
        "April", "May", "June", "July",
        "August", "September", "October",
        "November", "December",
    };
    chrono::sys_days days_;
};

static_assert(std::weakly_incrementable<Date>);

///////////////////////////////////////////////////////////////////////////////
// group_by_view
template<ranges::viewable_range Rng, typename Pred>
struct group_by_view: ranges::view_interface<group_by_view<Rng, Pred>> {
    group_by_view() = default;
    group_by_view(Rng r, Pred p): r(std::move(r)), p(std::move(p)) {}

private:
    struct GroupIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = ranges::subrange<ranges::iterator_t<Rng>>;

        GroupIterator& operator++() {
            cur = next_cur;
            if (cur != last) {
                next_cur = ranges::find_if_not(ranges::next(cur), last, [&](auto&& elem)
                                                                        { return p(*cur, elem); });
            }
            return *this;
        }
        GroupIterator operator++(int) {
            GroupIterator tmp(*this);
            ++*this;
            return tmp;
        }
        value_type operator*() const { return {cur, next_cur}; }

        friend bool operator==(const GroupIterator& iter, std::default_sentinel_t)
        { return iter.cur == iter.last; }

        // C++20 defaulted comparison operators
        friend bool operator==(const GroupIterator& lhs, const GroupIterator& rhs) = default;

        Pred p;
        ranges::iterator_t<Rng> cur{};
        ranges::iterator_t<Rng> next_cur{};
        ranges::sentinel_t<Rng> last{};
    };
public:

    GroupIterator begin() {
        auto beg = ranges::begin(r);
        auto end = ranges::end(r);
        return {p, beg,
            ranges::find_if_not(ranges::next(beg), end,
                                [&](auto&& elem) { return p(*beg, elem); }),
                end};
    }

    std::default_sentinel_t end() { return {}; }

    Rng r;
    Pred p;
};

#if __GNUC__ > 10
struct _Group_by: views::__adaptor::_RangeAdaptor<_Group_by> {
    template<ranges::viewable_range Rng, typename Pred>
    constexpr auto operator()(Rng&& r, Pred&& p) const {
        return group_by_view{std::forward<Rng>(r), std::forward<Pred>(p)};
    }
    static constexpr int _S_arity = 2;
    using views::__adaptor::_RangeAdaptor<_Group_by>::operator();
};
inline constexpr _Group_by group_by;
#else
inline constexpr views::__adaptor::_RangeAdaptor group_by
    = [] <ranges::viewable_range Rng, typename Pred> (Rng&& r, Pred&& p) {
        return group_by_view{std::forward<Rng>(r), std::forward<Pred>(p)};
};
#endif


///////////////////////////////////////////////////////////////////////////////
// concat_view
template<ranges::viewable_range... Rngs>
requires (sizeof...(Rngs) > 1)
struct concat_view {
    static constexpr size_t nRngs = sizeof...(Rngs);
    using RNGs = std::tuple<Rngs...>;
    concat_view() = default;
    explicit concat_view(Rngs... rngs): rngs{std::move(rngs)...}
    { }

    struct ConcatIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = std::common_type_t<ranges::range_reference_t<Rngs>...>;

        ConcatIterator() = default;
        ConcatIterator(RNGs* rngs): rngs(rngs)
        { its.template emplace<0>(ranges::begin(std::get<0>(*rngs))); }

        ConcatIterator& operator++() {
            // TODO: check empty range, skip it
            std::visit([&](auto&& iter) {
                constexpr size_t idx = iter.value;
                if ( (iter.iterator = ranges::next(iter.iterator))
                        == ranges::end(std::get<idx>(*rngs)) ) {
                    if constexpr (idx + 1 < nRngs) {
                        its.template emplace<idx + 1>(
                            ranges::begin(std::get<idx + 1>(*rngs))
                        );
                    }
                }
            }, its);
            return *this;
        }
        ConcatIterator operator++(int) {
            ConcatIterator tmp(*this);
            ++*this;
            return tmp;
        }
        using reference = std::common_reference_t<ranges::range_reference_t<Rngs>...>;
        reference operator*() const {
            return std::visit([](auto&& iter) -> reference {
                return *iter.iterator;
            }, its);
        }

        friend bool operator==(const ConcatIterator& iter, std::default_sentinel_t) {
            return iter.its.index() == nRngs - 1 &&
                   (std::get<nRngs - 1>(iter.its).iterator ==
                    ranges::end(std::get<nRngs - 1>(*iter.rngs)));
        }

        friend bool operator==(const ConcatIterator& lhs, const ConcatIterator& rhs) = default;

    private:
        template<size_t N, typename Rng>
        struct IteratorWithIndex: std::integral_constant<size_t, N> {
            IteratorWithIndex() = default;
            IteratorWithIndex(ranges::iterator_t<Rng> iterator):
                iterator(std::move(iterator)) {}
            ranges::iterator_t<Rng> iterator;
            friend bool operator==(const IteratorWithIndex& lhs, const IteratorWithIndex& rhs) = default;
        };

        template<size_t ...Is>
        static constexpr auto iteratorVariantGenerator(std::index_sequence<Is...>)
            -> std::variant<IteratorWithIndex<Is, std::tuple_element_t<Is, RNGs>>...>;

        decltype(iteratorVariantGenerator(std::make_index_sequence<nRngs>{})) its;
        RNGs* rngs {};
    };

    ConcatIterator begin() { return {&this->rngs}; }

    std::default_sentinel_t end() { return {}; }

private:
    RNGs rngs;
};

inline constexpr auto concat = [] <ranges::viewable_range... Rngs> (Rngs&&... rngs) {
    return concat_view{std::forward<Rngs>(rngs)...};
};

///////////////////////////////////////////////////////////////////////////////
// repeat_n
template<typename Value>
struct repeat_n_view: ranges::view_interface<repeat_n_view<Value>> {
    repeat_n_view() = default;
    repeat_n_view(size_t n, Value value): n(n), value(std::move(value)) { }

private:
    struct RepeatIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = Value;

        RepeatIterator& operator++() { --n; return *this; }
        RepeatIterator operator++(int) {
            RepeatIterator tmp(*this);
            ++*this;
            return tmp;
        }
        decltype(auto) operator*() const { return *value; }

        friend bool operator==(const RepeatIterator& iter, std::default_sentinel_t)
        { return iter.n == 0; }

        friend bool operator==(const RepeatIterator& lhs, const RepeatIterator& rhs) = default;

        size_t n{};
        Value* value{};
    };

public:
    RepeatIterator begin() { return {n, &value}; }
    std::default_sentinel_t end() { return {}; }

private:
    size_t n;
    Value value;
};

inline constexpr auto repeat_n = []<typename Value>(Value&& value, size_t n) {
    return repeat_n_view{n, std::forward<Value>(value)};
};

///////////////////////////////////////////////////////////////////////////////
// join
#if __GNUC__ > 10
struct _Join: views::__adaptor::_RangeAdaptorClosure {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& rng) const {
        using RangeValue = ranges::range_value_t<Rng>;
        using JoinResult = std::conditional_t<
            ranges::range<RangeValue> && !ranges::view<RangeValue>, // is container?
            RangeValue,
            std::vector<ranges::range_value_t<RangeValue>>
        >;
        JoinResult res;
        for (auto&& r: rng) {
            res.insert(ranges::end(res),
                    ranges::begin(r),
                    ranges::end(r));
        }
        return res;
    }
};
inline constexpr _Join join;
#else
inline constexpr views::__adaptor::_RangeAdaptorClosure join
    = [] <ranges::viewable_range Rng> (Rng&& rng) {
    using RangeValue = ranges::range_value_t<Rng>;
    using JoinResult = std::conditional_t<
        ranges::range<RangeValue> && !ranges::view<RangeValue>, // is container?
        RangeValue,
        std::vector<ranges::range_value_t<RangeValue>>
    >;
    JoinResult res;
    for (auto&& r: rng) {
        res.insert(ranges::end(res),
                ranges::begin(r),
                ranges::end(r));
    }
    return res;
};
#endif

///////////////////////////////////////////////////////////////////////////////
// chunk_view
template<ranges::viewable_range Rng>
struct chunk_view: ranges::view_interface<chunk_view<Rng>> {
    chunk_view() = default;
    chunk_view(Rng r, size_t n): r(std::move(r)), n(n) {}

private:
    struct ChunkIterator {
        using difference_type = std::ptrdiff_t;
        using value_type = ranges::subrange<ranges::iterator_t<Rng>>;

        ChunkIterator& operator++() {
            cur = next_cur;
            if (cur != last) {
                next_cur = ranges::next(cur, n, last);
            }
            return *this;
        }
        ChunkIterator operator++(int) {
            ChunkIterator tmp(*this);
            ++*this;
            return tmp;
        }
        value_type operator*() const { return {cur, next_cur}; }

        friend bool operator==(const ChunkIterator& iter, std::default_sentinel_t)
        { return iter.cur == iter.last; }

        friend bool operator==(const ChunkIterator& lhs, const ChunkIterator& rhs) {
            return lhs.cur == rhs.cur;
        }

        size_t n{};
        ranges::iterator_t<Rng> cur{};
        ranges::iterator_t<Rng> next_cur{};
        ranges::sentinel_t<Rng> last{};
    };
public:

    ChunkIterator begin() {
        auto beg = ranges::begin(r);
        auto end = ranges::end(r);
        return {n, beg, ranges::next(beg, n, end), end};
    }

    std::default_sentinel_t end() { return {}; }

    Rng r;
    size_t n;
};

#if __GNUC__ > 10
struct _Chunk: views::__adaptor::_RangeAdaptor<_Chunk> {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& r, size_t n) const {
        return chunk_view{std::forward<Rng>(r), n};
    }
    static constexpr int _S_arity = 2;
    using views::__adaptor::_RangeAdaptor<_Chunk>::operator();
};
inline constexpr _Chunk chunk;
#else
inline constexpr views::__adaptor::_RangeAdaptor chunk
    = [] <ranges::viewable_range Rng> (Rng&& r, size_t n) {
        return chunk_view{std::forward<Rng>(r), n};
};
#endif


///////////////////////////////////////////////////////////////////////////////
// interleave_view
template<ranges::viewable_range Rng>
struct interleave_view: ranges::view_interface<interleave_view<Rng>> {
    using RNGs = std::vector<ranges::range_value_t<Rng>>;
    interleave_view() = default;
    interleave_view(Rng rng) {
        for(auto&& r: rng) { rngs.emplace_back(r); }
    }

private:
    struct InterleaveInterator {
        using difference_type = std::ptrdiff_t;
        using value_type = ranges::range_value_t<ranges::range_value_t<Rng>>;

        InterleaveInterator() = default;
        InterleaveInterator(RNGs* rngs): rngs(rngs) {
            for (auto&& r: *rngs) {
                its.emplace_back(ranges::begin(r));
            }
        }

        InterleaveInterator& operator++() {
            if ( ((++n) %= its.size()) == 0) {
                ranges::for_each(its, [](auto&& iter) {
                    iter = ranges::next(iter);
                });
            }
            return *this;
        }
        InterleaveInterator operator++(int) {
            InterleaveInterator tmp(*this);
            ++*this;
            return tmp;
        }
        decltype(auto) operator*() const { return *its[n]; }

        friend bool operator==(const InterleaveInterator& iter, std::default_sentinel_t) {
            if (iter.n != 0) return false;
            auto ends = *iter.rngs | views::transform(ranges::end);
            return ranges::mismatch(iter.its, ends, std::equal_to<>{}).in1 == iter.its.end();
        }

        friend bool operator==(const InterleaveInterator& lhs, const InterleaveInterator& rhs) = default;

        size_t n{};
        std::vector<ranges::iterator_t<ranges::range_value_t<Rng>>> its;
        RNGs* rngs{};
    };
public:

    InterleaveInterator begin() { return {&rngs}; }
    std::default_sentinel_t end() { return {}; }

    RNGs rngs;
};

#if __GNUC__ > 10
struct _Interleave: views::__adaptor::_RangeAdaptorClosure {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& r) const {
        return interleave_view{std::forward<Rng>(r)};
    }
};
inline constexpr _Interleave interleave;
#else
inline constexpr views::__adaptor::_RangeAdaptorClosure interleave
    = [] <ranges::viewable_range Rng> (Rng&& r) {
        return interleave_view{std::forward<Rng>(r)};
};
#endif

///////////////////////////////////////////////////////////////////////////////
// transpose
#if __GNUC__ > 10
struct _Transpose: views::__adaptor::_RangeAdaptorClosure {
    template<ranges::viewable_range Rng>
    constexpr auto operator()(Rng&& r) const {
        return r | interleave | chunk(ranges::distance(r));
    }
};
inline constexpr _Transpose transpose;
#else
inline constexpr views::__adaptor::_RangeAdaptorClosure transpose
    = [] <ranges::viewable_range Rng> (Rng&& r) {
    return r | interleave | chunk(ranges::distance(r));
};
#endif

///////////////////////////////////////////////////////////////////////////////

auto datesBetween(unsigned short start, unsigned short stop) {
    return views::iota(Date{start, 1, 1}, Date{stop, 1, 1});
}

auto by_month() {
    return group_by([](Date a, Date b) { return a.month() == b.month(); });
}

auto by_week() {
    return group_by([](Date a, Date b) { return a.weekdaylessThan(b); });
}

// TODO: c++20 std::format
std::string month_title(const Date& d) {
    std::string title(22, ' ');
    std::string_view name = d.monthName();
    ranges::copy(name, ranges::begin(title) + (22 - name.length()) / 2);
    return title;
}

// TODO: c++20 std::format
std::string format_day(const Date& d) {
    char res[4];
    sprintf(res, "%3d", (int)d.day());
    return res;
}

// in: range<range<Date>>
// out: range<std::string>
auto format_weeks() {
    return views::transform([](/*range<Date>*/auto&& week) {
        std::string weeks( (*ranges::begin(week)).dayOfWeek() * 3, ' ');
        weeks += (week | views::transform(format_day) | join);
        while (weeks.length() < 22) weeks.push_back(' ');
        return weeks;
    });
}

// in: range<range<Date>>
// out: range<range<std::string>>
auto layout_months() {
    return views::transform([](/*range<Date>*/ auto&& month) {
        auto week_count = ranges::distance(month | by_week());
        return concat(
            views::single(month_title(*ranges::begin(month))),
            views::single(std::string(" Su Mo Tu We Th Fr Sa ")),
            month | by_week() | format_weeks(),
            repeat_n(std::string(22, ' '), 6 - week_count)
        );
    });
}

// In:  range<range<range<string>>>
// Out: range<range<range<string>>>, transposing months.
auto transpose_months() {
    return views::transform([](/*range<range<string>>*/ auto&& rng) {
        return rng | transpose;
    });
}

// In:  range<range<string>>
// Out: range<string>, joining the strings of the inner ranges
auto join_months()
{
    return views::transform(
        [](/*range<string>*/ auto rng) { return join(rng); });
}

int main(int argc, char** argv) {
    auto formattedCalendar
        = datesBetween(2021, 2022) // range<Date>: 356
        | by_month()               // range<range<Date>>: 12 x month
        | layout_months()          // range<range<std::string>>: 12 x 8 x 22
        | chunk(3)                 // range<range<range<std::string>>>: 4 x 3 x 8 x 22
        | transpose_months()       // range<range<range<std::string>>>: 4 x 8 x 3 x 22
        | views::join              // range<range<std::string>>: 32 x 3 x 22
        | join_months()            // range<std::string>: 32 x 66
        ;

    ranges::copy(formattedCalendar,
                std::ostream_iterator<std::string_view>(std::cout, "\n"));

    return 0;
}
