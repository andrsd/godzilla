// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Terminal.h"
#include "fmt/core.h"
#include <source_location>

namespace godzilla {

inline void
assert_true(bool cond,
            fmt::string_view format,
            std::source_location loc = std::source_location::current())
{
#ifndef NDEBUG
    if (!cond) {
        fmt::print(stderr, "{}", Terminal::red);
        fmt::print(stderr,
                   "Assertion failed: {}, in function '{}' at {}:{}",
                   format,
                   loc.function_name(),
                   loc.file_name(),
                   loc.line());
        fmt::println(stderr, "{}", Terminal::normal);
        std::abort();
    }
#endif
}

template <typename... T>
inline void
expect_true(bool cond, fmt::format_string<T...> format, T... args)
{
    if (!cond) {
        fmt::print(stderr, "{}", Terminal::red);
        fmt::print(stderr, format, std::forward<T>(args)...);
        fmt::println(stderr, "{}", Terminal::normal);
        std::abort();
    }
}

} // namespace godzilla
