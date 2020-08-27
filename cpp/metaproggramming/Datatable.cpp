/*************************************************************************
    > File Name: Datatable.cpp
    > Author: Netcan
    > Descripton: constexpr Datatable
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-14 21:16
************************************************************************/
#include <algorithm>
#include <bitset>
#include <cstdio>
#include <gtest/gtest.h>
#include "Typelist.hpp"

template<typename ES = TypeList<>, typename GS = TypeList<>, typename = void>
struct GroupEntriesTrait {
    using type = GS;
};

template<typename ES, typename GS>
class GroupEntriesTrait<ES, GS, std::void_t<typename ES::head>> {
    template<typename E>
    struct GroupPrediction {
        static constexpr bool value =
            ES::head::dim == E::dim &&
            sizeof(typename ES::head::type) == sizeof(typename E::type) &&
            alignof(typename ES::head::type) == alignof(typename E::type);
    };
    using group = Partition_t<ES, GroupPrediction>;
    using satisfied = typename group::satisfied;
    using rest = typename group::rest;
public:
    using type = typename GroupEntriesTrait<rest, typename GS::template appendTo<satisfied>>::type;
};

template<typename IN, auto GROUP_N = 0, typename INDEX = TypeList<>, auto N = 0, typename = void>
struct RegionIndexTrait {
    using type = INDEX;
};

template<typename IN, typename INDEX, auto GROUP_N, auto N>
class RegionIndexTrait<IN, GROUP_N, INDEX, N, std::void_t<typename IN::head>> {
    struct entry {
        static constexpr auto key = IN::head::key;
        static constexpr auto id = (GROUP_N << 16) | N;
    };
    using entries = typename INDEX::template appendTo<entry>;
public:
    using type = typename RegionIndexTrait<typename IN::tails, GROUP_N, entries, N+1>::type;
};

template<typename IN, typename INDEX = TypeList<>, auto GROUP_N = 0, typename = void>
struct GroupIndexTrait {
    using type = INDEX;
};

template<typename IN, typename INDEX, auto GROUP_N>
class GroupIndexTrait<IN, INDEX, GROUP_N, std::void_t<typename IN::head>> {
    using index = typename RegionIndexTrait<typename IN::head, GROUP_N, INDEX>::type;
public:
    using type = typename GroupIndexTrait<typename IN::tails, index, GROUP_N + 1>::type;
};

// <key, valuetype>
template<auto KEY, typename T, size_t DIM = 1>
struct Entry {
    static constexpr auto key = KEY;
    static constexpr size_t dim = DIM;
    static constexpr bool isArray = DIM > 1;
    using type = T;
};

template<auto KEY, typename T, size_t DIM>
struct Entry<KEY, T[DIM]>: Entry<KEY, T, DIM> { };

template <typename ES>
class Datatable {
    struct Region {
        virtual auto getData(size_t index, void* out, size_t len) -> bool = 0;
        virtual auto setData(size_t index, const void* value, size_t len) -> bool = 0;
        virtual ~Region() = default;
    };

    template<typename EH, typename ...ET>
    class GenericRegion: public Region {
        static constexpr size_t numberOfEntries = sizeof...(ET) + 1;
        static constexpr size_t alignSize = alignof(typename EH::type);
        static constexpr size_t typeSize = sizeof(typename EH::type);
        static constexpr size_t dim = EH::dim;
        static constexpr size_t maxSize = std::max(alignSize, typeSize) * dim;
        std::bitset<numberOfEntries> mask;
        char table[numberOfEntries][maxSize] = {};
    public:
        auto getData(size_t index, void* out, size_t len) -> bool override {
            if (index >= numberOfEntries || ! mask[index]) { return false; }
            std::copy_n(table[index], std::min(len, maxSize), reinterpret_cast<char*>(out));
            return true;
        }
        auto setData(size_t index, const void* value, size_t len) -> bool override {
            if (index >= numberOfEntries) { return false; }
            std::copy_n(reinterpret_cast<const char*>(value), std::min(len, maxSize), table[index]);
            mask[index] = true;
            return true;
        }
    };

    template<typename... R>
    class Regions: private R... {
        Region* regions[sizeof...(R)];
        template<typename T>
        auto forData(int32_t index, T* out, size_t len, bool (Region::*op)(size_t, T*, size_t)) -> bool {
            size_t regionIdx = index >> 16;
            if (regionIdx >= sizeof...(R)) { return false; }
            return (regions[regionIdx]->*op)(index & 0xFFFF, out, len);
        };
    public:
        constexpr Regions() {
            int i = 0;
            (void(regions[i++] = static_cast<R*>(this)), ...);
        }
        auto getData_(size_t index, void* out, size_t len) -> bool {
            return forData(index, out, len, &Region::getData);
        }
        auto setData_(size_t index, const void* value, size_t len) -> bool {
            return forData(index, value, len, &Region::setData);
        }
    };

    template<typename GS, typename RS = TypeList<>, typename = void>
    struct GenericRegionTrait {
        using type = RS;
    };

    template<typename GS, typename RS>
    class GenericRegionTrait<GS, RS, std::void_t<typename GS::head>> {
        using region = typename GS::head::template exportTo<GenericRegion>;
    public:
        using type = typename GenericRegionTrait<typename GS::tails, typename RS::template appendTo<region>>::type;
    };

    template<typename ...IS>
    struct IndexerTrait {
        size_t index[sizeof...(IS)];
        constexpr IndexerTrait() {
            (void(index[(IS::key >= sizeof...(IS)) ? -1 : IS::key] = IS::id), ...);
        }
    };

    using GS = typename GroupEntriesTrait<ES>::type;
    using INDEX = typename GroupIndexTrait<GS>::type;
    using RegionsClass = typename GenericRegionTrait<GS>::type::template exportTo<Regions>;
    using IndexerClass = typename INDEX::template exportTo<IndexerTrait>;

    RegionsClass regions;
    IndexerClass indexer;
public:
    auto getData(int32_t index, void* out, size_t len = -1) -> bool {
        if (index >= INDEX::size) { return false; }
        return regions.getData_(indexer.index[index], out, len);
    }
    auto setData(int32_t index, const void* value, size_t len = -1) -> bool {
        if (index >= INDEX::size) { return false; }
        return regions.setData_(indexer.index[index], value, len);
    }
};

TEST(TestDataTbl, get_and_set) {
    using AllEntries = TypeList<
        Entry<0, int>,
        Entry<1, char>,
        Entry<2, char>,
        Entry<3, short>,
        Entry<4, char[10]>,
        Entry<5, char[10]>,
        Entry<6, int> >;

    Datatable<AllEntries> datatbl;
    {
        int expectedValue = 23;
        EXPECT_FALSE(datatbl.getData(0, &expectedValue));
        EXPECT_TRUE(datatbl.setData(0, &expectedValue));
        int value = -1;
        EXPECT_TRUE(datatbl.getData(0, &value));
        EXPECT_EQ(value, expectedValue);
    }

    {
        const char* expectedValue = "hello";
        char value[10] {};
        EXPECT_FALSE(datatbl.getData(4, value));
        EXPECT_TRUE(datatbl.setData(4, expectedValue, strlen(expectedValue)));
        EXPECT_TRUE(datatbl.getData(4, value));

        EXPECT_STREQ(value, expectedValue);
    }

}
