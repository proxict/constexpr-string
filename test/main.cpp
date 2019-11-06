#include <constexpr-string/constexpr-string.hpp>

#include <cstdio>

struct IsLowerCase {
    constexpr bool operator()(const char c) const { return c == ' ' || (c >= 'a' && c <= 'z'); }
};

struct IsHexChar {
    constexpr bool operator()(const char c) const {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }
};

int main() {
    constexpr auto hexStr = String("8dd42e58184249a1974295b4bb97de9a");
    static_assert((hexStr.size() & 1) == 0, "String must have even length");
    static_assert(hexStr.allOf(IsHexChar()), "String may contain hex chars only");

    constexpr auto str = String("this is a test");
    static_assert(str.size() == 14, "Size failed");
    static_assert(str.allOf(IsLowerCase()), "Everything must be lowercase");

    constexpr auto prefix = String("I think ");
    constexpr auto postfix = String(" string.");
    static constexpr auto sentence = prefix + str + postfix;

    static_assert(String("ABC") + "def" + String("Ghi") == String("ABCdefGhi"), "Concatenation failed");
    static_assert(sentence.size() == prefix.size() + str.size() + postfix.size(), "Concatenation failed");
    static_assert(sentence == String("I think this is a test string."), "");
    static_assert(sentence[sentence.size()] == 0, "String must be null terminated");

    static_assert(sentence.substr<0, 1>() == String("I"), "substr failed");
    static_assert(sentence.substr<8, 22>() == String("this is a test"), "substr failed");
    static_assert(sentence.substr<23>() == String("string."), "substr failed");

    static_assert(sentence.find('I') == 0, "find failed");
    static_assert(sentence.find('t') == 2, "find failed");
    static_assert(sentence.find('!') == StringNpos, "find failed");
    static_assert(sentence.find('.') == sentence.size() - 1, "find failed");

    static_assert(sentence.find('k', 6) == 6, "find failed");
    static_assert(sentence.find('k', 7) == StringNpos, "find failed");
    static_assert(sentence.find('t', 0) == 2, "find failed");
    static_assert(sentence.find('t', 10) == 18, "find failed");
    static_assert(sentence.find("think", 0) == 2, "find failed");
    static_assert(sentence.find("think", 3) == StringNpos, "find failed");
    static_assert(sentence.find("is", 5) == 10, "find failed");
    static_assert(sentence.find("is", 11) == 13, "find failed");

    static_assert(sentence.find("abc") == StringNpos, "find failed");
    static_assert(sentence.find("I think") == 0, "find failed");
    static_assert(sentence.find("think") == 2, "find failed");

    static_assert(sentence.substr<8, 22>().find(" is a test") == 4, "find failed");
    static_assert(sentence.substr<8, 22>().find("is a test") == 5, "find failed");
    static_assert(sentence.substr<8, 22>().find("s a test") == 6, "find failed");

    static_assert(toLower('A') == 'a', "toLower failed");
    static_assert(toLower('Z') == 'z', "toLower failed");
    static_assert(toLower('a') == 'a', "toLower failed");
    static_assert(toLower(' ') == ' ', "toLower failed");
    static_assert(toLower('~') == '~', "toLower failed");

    static_assert(toUpper('a') == 'A', "toUpper failed");
    static_assert(toUpper('z') == 'Z', "toUpper failed");
    static_assert(toUpper('A') == 'A', "toUpper failed");
    static_assert(toUpper(' ') == ' ', "toUpper failed");
    static_assert(toUpper('~') == '~', "toUpper failed");

    static_assert(toLower(String("lower")) == String("lower"), "toLower failed");
    static_assert(toLower(String("UPPER")) == String("upper"), "toLower failed");
    static_assert(toLower(String("ToLower")) == String("tolower"), "toLower failed");

    static_assert(toUpper(String("lower")) == String("LOWER"), "toUpper failed");
    static_assert(toUpper(String("UPPER")) == String("UPPER"), "toUpper failed");
    static_assert(toUpper(String("ToUpper")) == String("TOUPPER"), "toUpper failed");

    puts("Passed");
}
