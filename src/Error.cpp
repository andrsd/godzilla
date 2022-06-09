#include "Error.h"
#include "CallStack.h"
#include <cstdio>

namespace godzilla {
namespace internal {

void
fprintf(std::ostream & os, const char * s)
{
    while (s && *s) {
        if (*s == '%' && *++s != '%')
            throw std::runtime_error("invalid format: missing arguments");
        os << *s++;
    }
}

void
print_call_stack()
{
    fprintf(std::cerr, "\n");
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
mem_check(int line, const char * func, const char * file, void * var)
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
