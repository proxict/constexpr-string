![GitHub](https://img.shields.io/github/license/proxict/constexpr-string)

constexpr-string
------------

This library provides a `ConstexprString` class which gives you the possibility to work with your strings in compile-time.
 - Written in C++11

The class implements:
 - concatenation with other strings
 - replace
 - substr
 - find
 - rfind
 - findFirstOf
 - findFirstNotOf
 - findLastOf
 - findLastNotOf
 - allOf
 - iterator over all string characters
 - equality comparator

Non-member functions:
 - cstrlen
 - cstreq
 - toLower
 - toUpper
 - cmin
 
Integration with CMake
----------------------------
```cmake
add_subdirectory(external/constexpr-string)
target_link_libraries(your-target
    PRIVATE constexpr-string
)
```

Tests can be allowed by setting `BUILD_TESTS` variable to `TRUE`:
```bash
mkdir -p build && cd build
cmake -DBUILD_TESTS=1 ..
```

How to use?
-----------
Just include the header and you're ready to go:
```c++
#include <iostream>
#include <constexpr-string/constexpr-string.hpp>

struct IsHexChar {
    constexpr bool operator()(const char c) const {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }
};

int main() {
    constexpr auto tale = String("Let me tell you a tale");
    static_assert("Listen. " + tale + ", ok?" == String("Listen. Let me tell you a tale, ok?"), "String concatenation");
    static_assert(tale.replace('l', 'k') == String("Let me tekk you a take"), "Char replace");
    static_assert(tale.substr<7, 15>() == String("tell you"), "String substr");
    static_assert(tale.find('y') == 12, "String find");
    static_assert(tale.find('t', 4) == 7, "String find");
    static_assert(tale.find("tell") == 7, "String find");
    static_assert(tale.find("story") == StringNpos, "String find");
    static_assert(tale.rfind('l') == 20, "String rfind");
    static_assert(tale.findFirstOf("tlf") == 2, "String findFirstOf");
    static_assert(tale.findLastOf("tlf") == 20, "String findLastOf");
    static_assert(tale.findFirstNotOf("Let me") == 9, "String findFirstNotOf");
    static_assert(tale.findLastNotOf("tale ") == 14, "String findLastNotOf");
    static_assert(toLower(tale) == String("let me tell you a tale"), "toLower");
    static_assert(toUpper(tale) == String("LET ME TELL YOU A TALE"), "toUpper");
    constexpr auto hexStr = String("8dd42e58184249a1974295b4bb97de9a");
    static_assert((hexStr.size() & 1) == 0, "This string must have even length");
    static_assert(hexStr.allOf(IsHexChar()), "This string may contain hex chars only");
    std::cout << tale.cStr() << std::endl;
}
```
