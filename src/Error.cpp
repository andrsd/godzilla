#include "Error.h"
#include "CallStack.h"
#include "Terminal.h"

namespace godzilla {
namespace internal {

void
godzillaMsgRaw(const std::string & msg)
{
    std::cout << msg << std::endl;
}

std::string
godzillaMsgFmt(const std::string & msg, const std::string & title, const Terminal::Color & color)
{
    std::ostringstream oss;
    oss << color << title << ": " << msg << Terminal::Color::normal << std::endl;
    return oss.str();
}

void
godzillaStreamAll(std::ostringstream &)
{
}

void
godzillaErrorRaw(std::string msg, bool call_stack)
{
    msg = godzillaMsgFmt(msg, "error", Terminal::Color::red);
    std::cerr << msg << std::flush;

    if (call_stack) {
        std::cerr << std::endl;
        getCallstack().dump();
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
memCheck(int line, const char * func, const char * file, void * var)
{
    if (var == nullptr) {
        std::ostringstream oss;
        internal::godzillaStreamAll(oss, "Out of memory");
        oss << std::endl;
        internal::godzillaStreamAll(oss, "  Location: ", file, ":", line);
        internal::godzillaErrorRaw(oss.str(), true);
        terminate();
    }
}

} // namespace internal

void
checkPetscError(PetscErrorCode ierr)
{
    if (ierr) {
        std::ostringstream oss;
        internal::godzillaStreamAll(oss, "PETSc error: ", ierr);
        internal::godzillaErrorRaw(oss.str(), true);
        internal::terminate();
    }
}

} // namespace godzilla
