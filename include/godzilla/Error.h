// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Terminal.h"
#include "fmt/core.h"
#include <mutex>
#include <source_location>
#include <ranges>

namespace godzilla {

/// Abort the run in MPI-friendly fashion
[[noreturn]] void abort();

namespace internal {

void mem_check(int line, const char * func, const char * file, void * var);

void check_petsc_error(int ierr, const char * file, int line);

template <typename... T>
void
print_msg(Terminal::Color color, const char * type, fmt::format_string<T...> format, T &&... args)
{
    fmt::print(stderr, "{}", color);
    fmt::print(stderr, "[{}] ", type);
    fmt::print(stderr, format, std::forward<T>(args)...);
    fmt::println(stderr, "{}", godzilla::Terminal::normal);
}

/// Report an error and terminate
///
/// @param format Formatting string
/// @param args Arguments for the formatting string
template <typename... T>
[[noreturn]] void
error(fmt::format_string<T...> format, T &&... args)
{
    print_msg(godzilla::Terminal::red, "ERROR", format, std::forward<T>(args)...);
    godzilla::abort();
}

/// Report a warning
///
/// @param format Formatting string
/// @param args Arguments for the formatting string
template <typename... T>
void
warning(fmt::format_string<T...> format, T &&... args)
{
    print_msg(godzilla::Terminal::yellow, "WARNING", format, std::forward<T>(args)...);
}

} // namespace internal

/// Check that memory allocation was ok. If not, report an error (also dump call stack) and
/// terminate
#define MEM_CHECK(var) godzilla::internal::mem_check(__LINE__, __PRETTY_FUNCTION__, __FILE__, var)

/// Check that PETSc call was successful . If not, report an error (also dump call stack) and
/// terminate
#define PETSC_CHECK(ierr) godzilla::internal::check_petsc_error(ierr, __FILE__, __LINE__)

enum class ErrorCode {
    ///
    NotFound,
    NotSet,
    OutOfRange
};

/// Calls `warning` but only once
///
/// @param args Arguments consumed by `warning`
#define warning_once(...)                                          \
    do {                                                           \
        static std::once_flag __once__;                            \
        std::call_once(__once__, [&]() { warning(__VA_ARGS__); }); \
    } while (0)

} // namespace godzilla
