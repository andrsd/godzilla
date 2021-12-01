#include "PrintInterface.h"
#include "Object.h"
#include "App.h"
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

} // namespace internal

PrintInterface::PrintInterface(const App & app) :
    verbosity_level(app.getVerbosityLevel()),
    prefix(""),
    pi_comm(app.getComm())
{
    _F_;
}

PrintInterface::PrintInterface(const Object * obj) :
    verbosity_level(dynamic_cast<const App &>(obj->getApp()).getVerbosityLevel()),
    prefix(obj->getName() + ": "),
    pi_comm(obj->comm())
{
    _F_;
}

/// Check PETSc error
///
/// @param ierr Error code returned by PETSc
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
