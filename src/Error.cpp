#include "Error.h"
#include "CallStack.h"
#include "Terminal.h"

namespace godzilla {
namespace internal {

void
godzilla_msg_raw(const std::string & msg)
{
    std::cout << msg << std::endl;
}

std::string
godzilla_msg_fmt(const std::string & msg, const std::string & title, const Terminal::Color & color)
{
    std::ostringstream oss;
    oss << color << title << ": " << msg << Terminal::Color::normal << std::endl;
    return oss.str();
}

void
godzilla_stream_all(std::ostringstream &)
{
}

void
godzilla_error_raw(std::string msg, bool call_stack)
{
    msg = godzilla_msg_fmt(msg, "error", Terminal::Color::red);
    std::cerr << msg << std::flush;

    if (call_stack) {
        std::cerr << std::endl;
        get_callstack().dump();
    }
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
        std::ostringstream oss;
        internal::godzilla_stream_all(oss, "Out of memory");
        oss << std::endl;
        internal::godzilla_stream_all(oss, "  Location: ", file, ":", line);
        internal::godzilla_error_raw(oss.str(), true);
        terminate();
    }
}

} // namespace internal

void
check_petsc_error(PetscErrorCode ierr)
{
    if (ierr) {
        std::ostringstream oss;
        internal::godzilla_stream_all(oss, "PETSc error: ", ierr);
        internal::godzilla_error_raw(oss.str(), true);
        internal::terminate();
    }
}

} // namespace godzilla
