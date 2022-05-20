#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "Terminal.h"
#include "petsc.h"

namespace godzilla {

namespace internal {

void fprintf(std::ostream & os, const char * s);

template <typename T, typename... Args>
void
fprintf(std::ostream & os, const char * s, T value, Args... args)
{
    while (s && *s) {
        if (*s == '%' && *(s + 1) != '%') {
            if (*(s + 1) == 'e')
                os << std::scientific;
            else if (*(s + 1) == 'f')
                os << std::defaultfloat;
            ++s;
            os << value;
            return fprintf(os, ++s, args...);
        }
        os << *s++;
    }
    throw std::runtime_error("extra arguments provided to fprintf");
}

template <typename... Args>
void
error_printf(const char * s, Args... args)
{
    fprintf(std::cerr, "%s", Terminal::Color::red);
    fprintf(std::cerr, "[ERROR] ");
    fprintf(std::cerr, s, std::forward<Args>(args)...);
    fprintf(std::cerr, "%s\n", Terminal::Color::normal);
}

/// Terminate the run
[[noreturn]] void terminate(int status = 1);

void mem_check(int line, const char * func, const char * file, void * var);

} // namespace internal

template <typename... Args>
[[noreturn]] void
error(const char * format, Args &&... args)
{
    internal::error_printf(format, std::forward<Args>(args)...);
    internal::terminate();
}

/// Check PETSc error
///
/// @param ierr Error code returned by PETSc
void check_petsc_error(PetscErrorCode ierr);

/// Check that memory allocation was ok. If not, report an error (also dump call stack) and
/// terminate
#define MEM_CHECK(var) godzilla::internal::mem_check(__LINE__, __PRETTY_FUNCTION__, __FILE__, var)

} // namespace godzilla
