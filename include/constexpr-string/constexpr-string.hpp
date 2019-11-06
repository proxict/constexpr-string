#ifndef PROXICT_CONSTEXPR_STRING_HPP_
#define PROXICT_CONSTEXPR_STRING_HPP_
#include <iostream>
#include <type_traits>

template <bool TTest, typename TType = void>
using EnableIf = typename std::enable_if<TTest, TType>::type;

template <int... Indices>
struct IntSequence {};

template <typename T>
struct append;

template <int... Indices>
struct append<IntSequence<Indices...>> {
    using type = IntSequence<Indices..., sizeof...(Indices)>;
};

template <int TSize>
struct MakeIntSequenceImpl;

template <>
struct MakeIntSequenceImpl<0> {
    using type = IntSequence<>;
};

template <int TSize>
using MakeIntSequence = typename MakeIntSequenceImpl<TSize>::type;

template <int TSize>
struct MakeIntSequenceImpl : append<MakeIntSequence<TSize - 1>> {
    static_assert(TSize >= 0, "Integer sequence size must be positive");
};

constexpr int cstrlen(const char* str) {
    return *str ? 1 + cstrlen(str + 1) : 0;
}

constexpr bool cstreq(const char* a, const char* b) {
    return *a == *b && (*a == 0 || cstreq(a + 1, b + 1));
}

template <int TSize>
class ConstexprString final {
public:
    static_assert(TSize >= -1, "Invalid string size");

    constexpr explicit ConstexprString(const char* data)
        : ConstexprString(data, MakeIntSequence<TSize>()) {}

    constexpr char operator[](const int index) const { return mData[index]; }

    template <int TOtherSize, typename = EnableIf<TOtherSize <= TSize>>
    constexpr ConstexprString(const ConstexprString<TOtherSize>& s1,
                              const ConstexprString<TSize - TOtherSize>& s2)
        : ConstexprString{ s1, s2, MakeIntSequence<TOtherSize>{}, MakeIntSequence<TSize - TOtherSize>{} } {}

    constexpr const char* cStr() const noexcept { return mData; }

    constexpr int size() const { return TSize; }

    template <typename TPredicate>
    constexpr bool allOf(TPredicate&& p) const {
        return allOfInternal(0, p);
    }

    template <int TOtherSize>
    constexpr ConstexprString<TSize + TOtherSize> operator+(const ConstexprString<TOtherSize>& rhs) const {
        return ConstexprString<TSize + TOtherSize>(*this, rhs);
    }

    template <int TFrom, int TTo = TSize, typename = EnableIf<TFrom <= TTo>>
    constexpr ConstexprString<TTo - TFrom> substr() const {
        return ConstexprString<TTo - TFrom>(mData + TFrom, MakeIntSequence<TTo - TFrom>{});
    }

    template <int TOtherSize>
    constexpr bool operator==(const ConstexprString<TOtherSize>& rhs) const {
        return size() == rhs.size() && cstreq(cStr(), rhs.cStr());
    }

    template <int TOtherSize>
    constexpr bool operator!=(const ConstexprString<TOtherSize>& rhs) const {
        return !(*this == rhs);
    }

    template <int TOtherSize, typename = EnableIf<TOtherSize - 1 <= TSize>>
    constexpr int find(char const (&str)[TOtherSize], const int pos = 0) const {
        return findInternal(pos, 0, str);
    }

    constexpr int find(const char c, const int pos = 0) const { return findInternal(pos, c); }

    constexpr const char* begin() const {
        return mData;
    }

    constexpr const char* end() const {
        return mData + TSize;
    }

private:
    constexpr int findInternal(int index, const char c) const {
        return index < TSize ? (mData[index] == c ? index : findInternal(index + 1, c)) : -1;
    }

    template <int TOtherSize>
    constexpr int findInternal(int index, int index2, char const (&str)[TOtherSize]) const {
        // clang-format off
        return index2 == TOtherSize - 1
                   ? index - TOtherSize + 1
                   : (index < TSize ? (mData[index] == str[index2] ? findInternal(index + 1, index2 + 1, str) : findInternal(index + 1 - index2, 0, str)) : -1);
        // clang-format on
    }

    template <int TNewSize>
    friend class ConstexprString;

    template <typename TPredicate>
    constexpr bool allOfInternal(const int index, TPredicate&& p) const {
        return index < TSize ? (p(mData[index]) ? allOfInternal(index + 1, p) : false) : true;
    }

    template <int... TPack>
    constexpr ConstexprString(const char* str, IntSequence<TPack...>)
        : mData{ str[TPack]..., '\0' } {}

    template <int TOtherSize, int... TPack1, int... TPack2>
    constexpr ConstexprString(const ConstexprString<TOtherSize>& lhs,
                              const ConstexprString<TSize - TOtherSize>& rhs,
                              IntSequence<TPack1...>,
                              IntSequence<TPack2...>)
        : mData{ lhs[TPack1]..., rhs[TPack2]..., '\0' } {}

    char mData[TSize + 1];
};

template <int TSize>
constexpr ConstexprString<TSize - 1> String(char const (&str)[TSize]) {
    return ConstexprString<TSize - 1>(str);
}

#endif // PROXICT_CONSTEXPR_STRING_HPP_
