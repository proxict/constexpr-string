#ifndef PROXICT_CONSTEXPR_STRING_HPP_
#define PROXICT_CONSTEXPR_STRING_HPP_
#include <iostream>
#include <type_traits>

template <bool TTest, typename TType = void>
using EnableIf = typename std::enable_if<TTest, TType>::type;

template <typename T, T... Indices>
struct IntSequence {};

template <typename T>
struct append;

template <typename T, T... Indices>
struct append<IntSequence<T, Indices...>> {
    using type = IntSequence<T, Indices..., sizeof...(Indices)>;
};

template <typename T, int TSize>
struct MakeIntSequenceImpl;

template <typename T>
struct MakeIntSequenceImpl<T, 0> {
    using type = IntSequence<T>;
};

template <typename T, int TSize>
using MakeIntSequence = typename MakeIntSequenceImpl<T, TSize>::type;

template <typename T, int TSize>
struct MakeIntSequenceImpl : append<MakeIntSequence<T, TSize - 1>> {
    static_assert(TSize >= 0, "Integer sequence size must be positive");
};

constexpr std::size_t cstrlen(const char* str) {
    return *str ? 1 + cstrlen(str + 1) : 0;
}

constexpr bool cstreq(const char* a, const char* b) {
    return *a == *b && (*a == 0 || cstreq(a + 1, b + 1));
}

constexpr char toLower(const char c) {
    return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

constexpr char toUpper(const char c) {
    return c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c;
}

template <typename T>
constexpr T cmin(const T& lhs, const T& rhs) {
    return rhs < lhs ? rhs : lhs;
}

static constexpr std::size_t StringNpos = 0xFFFFFFFF;

template <std::size_t TSize>
class ConstexprString final {
public:
    constexpr explicit ConstexprString(const char* str)
        : ConstexprString(str, MakeIntSequence<std::size_t, TSize>()) {}

    template <typename... TChars>
    constexpr ConstexprString(const char c, const TChars... chars)
        : mData{ c, chars... } {}

    constexpr char operator[](const std::size_t index) const { return mData[index]; }

    constexpr const char* cStr() const noexcept { return mData; }

    constexpr const char* operator()() const noexcept { return cStr(); }

    constexpr std::size_t size() const { return TSize; }

    constexpr bool empty() const { return TSize == 0; };

    template <typename TPredicate>
    constexpr bool allOf(TPredicate&& p) const {
        return allOfInternal(0, p);
    }

    template <std::size_t TOtherSize>
    constexpr ConstexprString<TSize + TOtherSize> operator+(const ConstexprString<TOtherSize>& rhs) const {
        return concatenate(
            *this, rhs, MakeIntSequence<std::size_t, TSize>{}, MakeIntSequence<std::size_t, TOtherSize>{});
    }

    template <std::size_t TFrom, std::size_t TTo = TSize, typename = EnableIf<TFrom <= TTo>>
    constexpr ConstexprString<cmin(TTo, TSize) - TFrom> substr() const {
        return ConstexprString<cmin(TTo, TSize) - TFrom>(mData + TFrom, MakeIntSequence<std::size_t, cmin(TTo, TSize) - TFrom>{});
    }

    template <std::size_t TOtherSize>
    constexpr bool operator==(const ConstexprString<TOtherSize>& rhs) const {
        return size() == rhs.size() && cstreq(cStr(), rhs.cStr());
    }

    template <std::size_t TOtherSize>
    constexpr bool operator!=(const ConstexprString<TOtherSize>& rhs) const {
        return !(*this == rhs);
    }

    constexpr std::size_t find(const char c, const std::size_t pos = 0) const {
        return pos < TSize ? (mData[pos] == c ? pos : find(c, pos + 1)) : StringNpos;
    }

    constexpr std::size_t rfind(const char c, const std::size_t pos = StringNpos) const {
        // clang-format off
        return pos == 0 ? (mData[cmin(pos, TSize - 1)] == c ? pos : StringNpos)
                        : mData[cmin(pos, TSize - 1)] == c ? cmin(pos, TSize - 1) : rfind(c, cmin(pos, TSize - 1) - 1);
        // clang-format on
    }

    template <std::size_t TOtherSize, typename = EnableIf<TOtherSize - 1 <= TSize>>
    constexpr std::size_t find(char const (&str)[TOtherSize], const std::size_t pos = 0) const {
        return pos < TSize ? (findSubstr(pos, 0, str) ? pos : find(str, pos + 1)) : StringNpos;
    }

    template <std::size_t TOtherSize, typename = EnableIf<TOtherSize - 1 <= TSize>>
    constexpr std::size_t rfind(char const (&str)[TOtherSize], const std::size_t pos = StringNpos) const {
        return pos == 0 ? (findSubstr(0, 0, str) ? pos : StringNpos)
                        : findSubstr(cmin(pos, TSize - 1), 0, str) ? cmin(pos, TSize - 1)
                                                               : rfind(str, cmin(pos, TSize - 1) - 1);
    }

    template <std::size_t TOtherSize>
    constexpr std::size_t findFirstOf(char const (&str)[TOtherSize],
                                      const std::size_t pos = 0,
                                      const std::size_t count = StringNpos) const {
        return pos < TSize ? (findAnyOf(pos, 0, str, count) ? pos : findFirstOf(str, pos + 1, count))
                           : StringNpos;
    }

    template <std::size_t TOtherSize>
    constexpr std::size_t findFirstNotOf(char const (&str)[TOtherSize],
                                      const std::size_t pos = 0,
                                      const std::size_t count = StringNpos) const {
        return pos < TSize ? (!findAnyOf(pos, 0, str, count) ? pos : findFirstNotOf(str, pos + 1, count))
                           : StringNpos;
    }

    template <std::size_t TOtherSize>
    constexpr std::size_t findLastOf(char const (&str)[TOtherSize],
                                     const std::size_t pos = StringNpos,
                                     const std::size_t count = StringNpos) const {
        return pos == 0 ? (findAnyOf(0, 0, str, count) ? 0 : StringNpos)
                        : findAnyOf(cmin(pos, TSize - 1), 0, str, count) ? cmin(pos, TSize - 1)
                                                                     : findLastOf(str, cmin(pos, TSize - 1) - 1, count);
    }

    template <std::size_t TOtherSize>
    constexpr std::size_t findLastNotOf(char const (&str)[TOtherSize],
                                     const std::size_t pos = StringNpos,
                                     const std::size_t count = StringNpos) const {
        return pos == 0 ? (!findAnyOf(0, 0, str, count) ? 0 : StringNpos)
                        : !findAnyOf(cmin(pos, TSize - 1), 0, str, count) ? cmin(pos, TSize - 1)
                                                                     : findLastNotOf(str, cmin(pos, TSize - 1) - 1, count);
    }

    constexpr ConstexprString<TSize> replace(const char c, const char cTo) const {
        return replaceInternal(c, cTo, MakeIntSequence<std::size_t, TSize>{});
    }

    constexpr const char* begin() const { return mData; }

    constexpr const char* end() const { return mData + TSize; }

private:
    template <std::size_t TOtherSize>
    constexpr bool findAnyOf(std::size_t index,
                             std::size_t index2,
                             char const (&str)[TOtherSize],
                             const std::size_t count = StringNpos) const {
        return index2 >= cmin(TOtherSize - 1, count)
                   ? false
                   : (mData[index] == str[index2] ? true : findAnyOf(index, index2 + 1, str, count));
    }

    template <std::size_t TOtherSize>
    constexpr bool findSubstr(std::size_t index, std::size_t index2, char const (&str)[TOtherSize]) const {
        // clang-format off
        return index2 == TOtherSize - 1 ? true :
            (index < TSize ? (mData[index] == str[index2] ? findSubstr(index + 1, index2 + 1, str) : false) : false);
        // clang-format on
    }

    template <std::size_t... TIndex>
    constexpr ConstexprString<TSize>
    replaceInternal(const char c, const char cTo, IntSequence<std::size_t, TIndex...>) const {
        return ConstexprString<TSize>((mData[TIndex] == c ? cTo : mData[TIndex])...);
    }

    template <std::size_t TNewSize>
    friend class ConstexprString;

    template <typename TPredicate>
    constexpr bool allOfInternal(const std::size_t index, TPredicate&& p) const {
        return index < TSize ? (p(mData[index]) ? allOfInternal(index + 1, p) : false) : true;
    }

    template <std::size_t... TIndex>
    constexpr ConstexprString(const char* str, IntSequence<std::size_t, TIndex...>)
        : mData{ str[TIndex]..., '\0' } {}

    template <std::size_t TSize1, std::size_t TSize2, std::size_t... TIndex1, std::size_t... TIndex2>
    static constexpr ConstexprString<TSize1 + TSize2> concatenate(const ConstexprString<TSize1>& lhs,
                                                                  const ConstexprString<TSize2>& rhs,
                                                                  IntSequence<std::size_t, TIndex1...>,
                                                                  IntSequence<std::size_t, TIndex2...>) {
        return ConstexprString<TSize1 + TSize2>(lhs[TIndex1]..., rhs[TIndex2]..., '\0');
    }

    char mData[TSize + 1];
};

template <std::size_t TSize1, std::size_t TSize2>
constexpr ConstexprString<TSize1 - 1 + TSize2> operator+(const char (&lhs)[TSize1],
                                                         const ConstexprString<TSize2>& rhs) {
    return ConstexprString<TSize1 - 1>(lhs) + rhs;
}

template <std::size_t TSize1, std::size_t TSize2>
constexpr ConstexprString<TSize1 + TSize2 - 1> operator+(const ConstexprString<TSize1>& lhs,
                                                         const char (&rhs)[TSize2]) {
    return lhs + ConstexprString<TSize2 - 1>(rhs);
}

template <std::size_t TSize>
constexpr ConstexprString<TSize - 1> String(char const (&str)[TSize]) {
    return ConstexprString<TSize - 1>(str);
}

namespace Detail {
template <std::size_t TSize, std::size_t... TIndex>
constexpr ConstexprString<TSize> toLowerImpl(const ConstexprString<TSize>& str,
                                             IntSequence<std::size_t, TIndex...>) {
    return ConstexprString<TSize>(toLower(str[TIndex])...);
}

template <std::size_t TSize, std::size_t... TIndex>
constexpr ConstexprString<TSize> toUpperImpl(const ConstexprString<TSize>& str,
                                             IntSequence<std::size_t, TIndex...>) {
    return ConstexprString<TSize>(toUpper(str[TIndex])...);
}
} // namespace Detail

template <std::size_t TSize>
constexpr ConstexprString<TSize> toLower(const ConstexprString<TSize>& str) {
    return Detail::toLowerImpl(str, MakeIntSequence<std::size_t, TSize>{});
}

template <std::size_t TSize>
constexpr ConstexprString<TSize> toUpper(const ConstexprString<TSize>& str) {
    return Detail::toUpperImpl(str, MakeIntSequence<std::size_t, TSize>{});
}

#endif // PROXICT_CONSTEXPR_STRING_HPP_
