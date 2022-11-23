#include "Error.h"
#include "CallStack.h"
#include <cstdio>
#include "fmt/printf.h"

namespace godzilla {
namespace internal {

void
print_call_stack()
{
    fmt::fprintf(stderr, "\n");
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
        error_printf("Out of memory");
        error_printf("");
        error_printf("  Location: %s:%d", file, line);
        print_call_stack();
        terminate();
    }
}

void
check_petsc_error(PetscErrorCode ierr, const char * file, int line)
{
    if (ierr) {
        error_printf("PETSc error: %d", ierr);
        error_printf("");
        error_printf("  Location: %s:%d", file, line);
        print_call_stack();
        terminate();
    }
}

} // namespace internal

} // namespace godzilla
