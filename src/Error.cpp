#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include <cstdio>
#include "fmt/printf.h"
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

void
mem_check(int line, const char *, const char * file, void * var)
{
    if (var == nullptr) {
        error_print("Out of memory");
        error_print("");
        error_print("  Location: {}:{}", file, line);
        print_call_stack();
        terminate();
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
        terminate();
    }
}

} // namespace internal

} // namespace godzilla
