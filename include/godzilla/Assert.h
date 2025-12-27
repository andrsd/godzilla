// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Error.h"
#include "godzilla/Terminal.h"
#include "fmt/core.h"
#include <source_location>

namespace godzilla {

#ifndef NDEBUG
inline void
assert_true(bool cond,
            fmt::string_view format,
            std::source_location loc = std::source_location::current())
{
    if (!cond) {
        fmt::print(stderr, "{}", Terminal::red);
        fmt::print(stderr,
                   "Assertion failed: {}, in function '{}' at {}:{}",
                   format,
                   loc.function_name(),
                   loc.file_name(),
                   loc.line());
        fmt::println(stderr, "{}", Terminal::normal);
        godzilla::abort();
    }
}

// clang-format off
    #define GODZILLA_ASSERT_TRUE(cond, msg)            \
        if (!(cond)) godzilla::assert_true(cond, msg);
// clang-format on

#else

[[deprecated]] inline void
assert_true(bool cond,
            fmt::string_view format,
            std::source_location loc = std::source_location::current())
{
}

    #define GODZILLA_ASSERT_TRUE(cond, msg)

#endif

template <typename... T>
inline void
expect_true(bool cond, fmt::format_string<T...> format, T... args)
{
    if (!cond) {
        fmt::print(stderr, "{}", Terminal::red);
        fmt::print(stderr, format, std::forward<T>(args)...);
        fmt::println(stderr, "{}", Terminal::normal);
        godzilla::abort();
    }
}

} // namespace godzilla
