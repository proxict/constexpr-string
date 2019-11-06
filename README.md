![GitHub](https://img.shields.io/github/license/proxict/constexpr-string)

constexpr-string
------------

This library provides a `ConstexprString` class which gives you the possibility to work with your strings in compile-time.
 - Written in C++11

The class implements:
 - concatenation with other strings
 - find
 - substr
 - iterator over all string characters
 - equality comparator
 
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
    constexpr auto hexStr = String("8dd42e58184249a1974295b4bb97de9a");
    static_assert((hexStr.size() & 1) == 0, "This string must have even length");
    static_assert(hexStr.allOf(IsHexChar()), "This string may contain hex chars only");
    std::cout << hexStr.cStr() << std::endl;
}
```
