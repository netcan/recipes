/* author: netcan
 * date: 2019/07/29 22:26
 * @shanghai
 */
#include <gtest/gtest.h>
#include "template/Relational.hpp"
#include "base/Construct.hpp"

using namespace base;

TEST(TestRelation, testRelation) {
    Relational<int> rel{1, 2};
    for(int k = 0; k < 10; ++k) {
        Relational<int> result = rel * k;
        EXPECT_EQ(result.GetNumerator(), k);
        EXPECT_EQ(result.GetDenominator(), 2);
        EXPECT_TRUE(rel * k == k * rel);
    }
}

TEST(TestConstruct, testConstruct) {
    struct HasConstruct {
        bool constructed;
        HasConstruct(bool constructed): constructed(constructed) {}
        RetCode OnConstructed() {
            if(!constructed) return SYSTEM_ERROR;
            return OK;
        }
    };

    struct HasNoConstruct {
    };

    HasConstruct constructOk(false);
    EXPECT_EQ(constructOk.constructed, false);

    auto pConstruct = Construct<HasConstruct>(true);
    EXPECT_NE(pConstruct, nullptr);
    delete pConstruct;

    pConstruct = Construct<HasConstruct>(false);
    EXPECT_EQ(pConstruct, nullptr);

    auto pNoConstruct = Construct<HasNoConstruct>();
    EXPECT_NE(pNoConstruct, nullptr);
    delete pNoConstruct;

    auto spConstruct = make_shared_construct<HasConstruct>(true);
    EXPECT_EQ(spConstruct.use_count(), 1);
}
