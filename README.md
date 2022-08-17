octo-wildcardmatching-cpp
==============

[![Wildcard Matching Linux Build Pipeline](https://github.com/ofiriluz/octo-wildcardmatching-cpp/actions/workflows/linux.yml/badge.svg)](https://github.com/ofiriluz/octo-wildcardmatching-cpp/actions/workflows/linux.yml)
[![Wildcard Matching Windows Build Pipeline](https://github.com/ofiriluz/octo-wildcardmatching-cpp/actions/workflows/windows.yml/badge.svg)](https://github.com/ofiriluz/octo-wildcardmatching-cpp/actions/workflows/windows.yml)
[![Wildcard Matching Mac Build Pipeline](https://github.com/ofiriluz/octo-wildcardmatching-cpp/actions/workflows/mac.yml/badge.svg)](https://github.com/ofiriluz/octo-wildcardmatching-cpp/actions/workflows/mac.yml)


Wildcard matching library for different purposes
Currently only supports Path matching
Supports single * for charchatrs and multi ** for path parts

Usage
=====

The usage is pretty straight forward:

```cpp
    octo::wildcardmatching::WildcardPathMatcher path_matcher;
    // Add wildcard paths that are allowed
    path_matcher.add_wildcard_paths({
        "/usr/**/*.so",
        "/home/*/*.json",
        "/home/*/*js",
        "**/.ssh",
        "/home/john/.bash*",
        "**/.bashrc",
        "**/test/test2.txt",
        "/etc/*",
        "**/a/**/b/**/c",
        "/single/*a*b*c",
        "/pre*a*b*post",
        "**/test3/**/test4/**",
        "**/test5/test6"
    });

    // Try and get a match from the list above
    std::string match = path_matcher.get_wildcard_match("/dev/tty");

    // Check if theres a match
    path_matcher.has_match("/some/path");
```
