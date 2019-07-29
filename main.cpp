/* author: netcan
 * date: 2019/07/29 22:26
 * @shanghai
 */
#include <gtest/gtest.h>
#include "Relational.hpp"

TEST(TestRelation, testRelation) {
    Relational<int> rel{1, 2};
    for(int k = 0; k < 10; ++k) {
        Relational<int> result = rel * k;
        EXPECT_EQ(result.GetNumerator(), k);
        EXPECT_EQ(result.GetDenominator(), 2);
        EXPECT_TRUE(rel * k == k * rel);
    }
}
