#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "Terminal.h"
#include "fmt/printf.h"
#include "fmt/color.h"

namespace godzilla {

namespace internal {

template <typename... T>
void
error_print(fmt::format_string<T...> format, T... args)
{
    fmt::print(stderr, "{}", (const char *) Terminal::Color::red);
    fmt::print(stderr, "[ERROR] ");
    fmt::print(stderr, format, std::forward<T>(args)...);
    fmt::print(stderr, "{}", (const char *) Terminal::Color::normal);
    fmt::print(stderr, "\n");
}

/// Terminate the run
[[noreturn]] void terminate(int status = 1);

void mem_check(int line, const char * func, const char * file, void * var);

void check_petsc_error(int ierr, const char * file, int line);

void check_mpi_error(int ierr, const char * file, int line);

} // namespace internal

template <typename... T>
[[noreturn]] void
error(fmt::format_string<T...> format, T... args)
{
    internal::error_print(format, std::forward<T>(args)...);
    internal::terminate();
}

/// Check that memory allocation was ok. If not, report an error (also dump call stack) and
/// terminate
#define MEM_CHECK(var) godzilla::internal::mem_check(__LINE__, __PRETTY_FUNCTION__, __FILE__, var)

/// Check that PETSc call was successful . If not, report an error (also dump call stack) and
/// terminate
#define PETSC_CHECK(ierr) godzilla::internal::check_petsc_error(ierr, __FILE__, __LINE__)

/// Check that MPI call was successful.
#define MPI_CHECK(ierr) godzilla::internal::check_mpi_error(ierr, __FILE__, __LINE__)

} // namespace godzilla
