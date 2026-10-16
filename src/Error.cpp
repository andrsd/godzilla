// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Terminal.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include "fmt/core.h"
#include "petsc.h"
#include "mpi.h"
#include <cstdio>

namespace mpi = mpicpp_lite;

namespace godzilla {
namespace internal {

/// Terminate the run
[[noreturn]] void
terminate(int status)
{
    MPI_Finalize();
    exit(status);
}

template <typename... T>
void
error_print(int rank, fmt::format_string<T...> format, T... args)
{
    fmt::print(stderr, "{}", Terminal::red);
    fmt::print(stderr, "[{}] [ERROR] ", rank);
    fmt::print(stderr, format, std::forward<T>(args)...);
    fmt::print(stderr, "{}", Terminal::normal);
    fmt::print(stderr, "\n");
}

void
mem_check(void * var, const std::source_location location)
{
    mpi::Communicator comm = PETSC_COMM_WORLD;

    if (var == nullptr) {
        auto rank = comm.rank();
        if (rank == 0) {
            error_print(rank, "Out of memory");
            fmt::print(stderr, "{}", Terminal::red);
            fmt::println(stderr, "  at {}:{}", location.file_name(), location.line());
            godzilla::print_call_stack();
            fmt::print(stderr, "{}", Terminal::normal);
        }
        terminate(1);
    }
}

void
check_petsc_error(int ierr, const std::source_location location)
{
    mpi::Communicator comm = PETSC_COMM_WORLD;

    if (ierr) {
        auto rank = comm.rank();
        if (rank == 0) {
            error_print(rank, "PETSc error: {}", ierr);
            fmt::print(stderr, "{}", Terminal::red);
            fmt::println(stderr, "  at {}:{}", location.file_name(), location.line());
            godzilla::print_call_stack();
            fmt::print(stderr, "{}", Terminal::normal);
        }
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
