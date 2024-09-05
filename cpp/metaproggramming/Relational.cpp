/* author: netcan
 * date: 2019/07/29 22:45
 * @shanghai
 */
#include <gtest/gtest.h>
#include "utils/Construct.hpp"
#include <functional>

template<typename T>
class Relational {
    public:
        Relational(T numerator_ = 0, T denominator_ = 1):
            numerator_(numerator_), denominator_(denominator_) { }
        T GetNumerator() const { return numerator_; }
        T GetDenominator() const { return denominator_; }
    private:
        T numerator_;
        T denominator_;

        friend const Relational operator*(const Relational& lhs,
                const Relational& rhs) { // implicit convert T to Relational
            return DoMultiply(lhs, rhs); // call function simply to suppress inlined, template instantiation
        }
        friend const bool operator==(const Relational& lhs,
                const Relational& rhs) {
            return DoEqual(lhs, rhs);
        }
};

template<typename T>
const Relational<T> DoMultiply(const Relational<T>& lhs,
        const Relational<T>& rhs) {
    return Relational<T>(lhs.GetNumerator() * rhs.GetNumerator(),
            lhs.GetDenominator() * rhs.GetDenominator());
}

template<typename T>
const bool DoEqual(const Relational<T>& lhs,
        const Relational<T>& rhs) {
    return lhs.GetNumerator() == rhs.GetNumerator() &&
            lhs.GetDenominator() == rhs.GetDenominator();
}

TEST(TestRelation, testRelation) {
    Relational<int> rel{1, 2};
    for(int k = 0; k < 10; ++k) {
        Relational<int> result = rel * k;
        EXPECT_EQ(result.GetNumerator(), k);
        EXPECT_EQ(result.GetDenominator(), 2);
        EXPECT_TRUE(rel * k == k * rel);
    }
}

