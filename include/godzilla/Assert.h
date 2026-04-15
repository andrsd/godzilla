// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Terminal.h"
#include "godzilla/Error.h"
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
        std::abort();
    }
}

// clang-format off
    #define GODZILLA_ASSERT_TRUE(cond, msg)            \
        if (!(cond)) godzilla::assert_true(cond, msg);
// clang-format on

#else

[[deprecated]] inline void
assert_true(bool, fmt::string_view, std::source_location = std::source_location::current())
{
}

    #define GODZILLA_ASSERT_TRUE(cond, msg)

#endif

/// Error checking function with a condition
///
/// @param cond Condition expected to be true
template <typename... T>
inline void
expect_true(bool cond,
            const std::string & error_msg,
            const std::source_location loc = std::source_location::current())
{
    if (!cond) {
        internal::print_msg(godzilla::Terminal::red, "ERROR", "{}", error_msg);
        fmt::print(stderr, "{}", godzilla::Terminal::red);
        fmt::println(stderr, "  at {} ({}:{})", loc.function_name(), loc.file_name(), loc.line());
        print_call_stack(loc);
        fmt::println(stderr, "{}", godzilla::Terminal::normal);
        std::exit(-1);
    }
}

} // namespace godzilla
