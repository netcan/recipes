/* author: netcan
 * date: 2019/07/29 22:26
 * @shanghai
 */
#include <gtest/gtest.h>
#include "utils/Construct.hpp"
#include <functional>

using namespace utils;

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
