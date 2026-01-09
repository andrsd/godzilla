// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Terminal.h"
#include "fmt/core.h"
#include <mutex>

namespace godzilla {

namespace internal {

void mem_check(int line, const char * func, const char * file, void * var);

void check_petsc_error(int ierr, const char * file, int line);

} // namespace internal

/// Abort the run in MPI-friendly fashion
[[noreturn]] void abort();

/// Check that memory allocation was ok. If not, report an error (also dump call stack) and
/// terminate
#define MEM_CHECK(var) godzilla::internal::mem_check(__LINE__, __PRETTY_FUNCTION__, __FILE__, var)

/// Check that PETSc call was successful . If not, report an error (also dump call stack) and
/// terminate
#define PETSC_CHECK(ierr) godzilla::internal::check_petsc_error(ierr, __FILE__, __LINE__)

enum class ErrorCode {
    ///
    NotFound,
    NotSet
};

/// Error checking function with a condition
///
/// @param cond Condition expected to be true
template <typename... T>
inline void
expect_true(bool cond, fmt::format_string<T...> format, T... args)
{
    if (!cond) {
        fmt::print(stderr, "{}", Terminal::red);
        fmt::print(stderr, "[ERROR] ");
        fmt::print(stderr, format, std::forward<T>(args)...);
        fmt::println(stderr, "{}", Terminal::normal);
        godzilla::abort();
    }
}

/// Report an error and terminate
///
/// @param format Formatting string
/// @param args Arguments for the formatting string
template <typename... T>
void
error(fmt::format_string<T...> format, T... args)
{
    fmt::print(stderr, "{}", godzilla::Terminal::red);
    fmt::print(stderr, "[ERROR] ");
    fmt::print(stderr, format, std::forward<T>(args)...);
    fmt::println(stderr, "{}", godzilla::Terminal::normal);
    godzilla::abort();
}

/// Report a warning
///
/// @param format Formatting string
/// @param args Arguments for the formatting string
template <typename... T>
void
warning(fmt::format_string<T...> format, T... args)
{
    fmt::print(stdout, "{}", godzilla::Terminal::yellow);
    fmt::print(stdout, "[WARNING] ");
    fmt::print(stdout, format, std::forward<T>(args)...);
    fmt::println(stdout, "{}", godzilla::Terminal::normal);
}

/// Calls `warning` but only once
///
/// @param args Arguments consumed by `warning`
#define warning_once(...)                                          \
    do {                                                           \
        static std::once_flag __once__;                            \
        std::call_once(__once__, [&]() { warning(__VA_ARGS__); }); \
    } while (0)

} // namespace godzilla
