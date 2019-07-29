/* author: netcan
 * date: 2019/07/29 22:45
 * @shanghai
 */
#pragma once
template<class T>
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

template<class T>
const Relational<T> DoMultiply(const Relational<T>& lhs,
        const Relational<T>& rhs) {
    return Relational<T>(lhs.GetNumerator() * rhs.GetNumerator(),
            lhs.GetDenominator() * rhs.GetDenominator());
}

template<class T>
const bool DoEqual(const Relational<T>& lhs,
        const Relational<T>& rhs) {
    return lhs.GetNumerator() == rhs.GetNumerator() &&
            lhs.GetDenominator() == rhs.GetDenominator();
}
