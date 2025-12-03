// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Terminal.h"
#include "fmt/core.h"
#include <cstdio>
#include "mpi.h"

namespace godzilla {
namespace internal {

void
print_call_stack()
{
    fmt::print(stderr, "\n");
    get_callstack().dump();
}

/// Terminate the run
[[noreturn]] void
terminate(int status)
{
    MPI_Finalize();
    exit(status);
}

template <typename... T>
void
error_print(fmt::format_string<T...> format, T... args)
{
    fmt::print(stderr, "{}", Terminal::red);
    fmt::print(stderr, "[ERROR] ");
    fmt::print(stderr, format, std::forward<T>(args)...);
    fmt::print(stderr, "{}", Terminal::normal);
    fmt::print(stderr, "\n");
}

void
mem_check(int line, const char *, const char * file, void * var)
{
    if (var == nullptr) {
        error_print("Out of memory");
        error_print("");
        error_print("  Location: {}:{}", file, line);
        print_call_stack();
        terminate(1);
    }
}

void
check_petsc_error(int ierr, const char * file, int line)
{
    if (ierr) {
        error_print("PETSc error: {}", ierr);
        error_print("");
        error_print("  Location: {}:{}", file, line);
        print_call_stack();
        terminate(1);
    }
}

} // namespace internal

[[noreturn]] void
abort()
{
    std::abort();
}

} // namespace godzilla
