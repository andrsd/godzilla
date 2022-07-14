#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "Terminal.h"
#include "petsc.h"
#include "fmt/printf.h"
#include "fmt/color.h"

namespace godzilla {

namespace internal {

template <typename... Args>
void
error_printf(const char * s, Args... args)
{
    fmt::fprintf(stderr, "%s", Terminal::Color::red);
    fmt::fprintf(stderr, "[ERROR] ");
    fmt::fprintf(stderr, s, std::forward<Args>(args)...);
    fmt::fprintf(stderr, "%s", Terminal::Color::normal);
    fmt::fprintf(stderr, "\n");
}

/// Terminate the run
[[noreturn]] void terminate(int status = 1);

void mem_check(int line, const char * func, const char * file, void * var);

void check_petsc_error(PetscErrorCode ierr, const char * file, int line);

} // namespace internal

template <typename... Args>
[[noreturn]] void
error(const char * format, Args &&... args)
{
    internal::error_printf(format, std::forward<Args>(args)...);
    internal::terminate();
}

/// Check that memory allocation was ok. If not, report an error (also dump call stack) and
/// terminate
#define MEM_CHECK(var) godzilla::internal::mem_check(__LINE__, __PRETTY_FUNCTION__, __FILE__, var)

/// Check that PETSc call was successful . If not, report an error (also dump call stack) and
/// terminate
#define PETSC_CHECK(ierr) godzilla::internal::check_petsc_error(ierr, __FILE__, __LINE__)

} // namespace godzilla
