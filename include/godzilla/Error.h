// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Terminal.h"
#include "fmt/core.h"

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
    NotFound
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

} // namespace godzilla
