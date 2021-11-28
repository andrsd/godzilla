#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "mpi.h"
#include "petscsys.h"
#include "CallStack.h"

namespace godzilla {

namespace internal {

/// All of the following are not meant to be called directly - they are called by the normal macros
/// (godzillaError(), etc.) down below
void godzillaStreamAll(std::ostringstream & ss);

template <typename T, typename... Args>
void
godzillaStreamAll(std::ostringstream & ss, T && val, Args &&... args)
{
    ss << val;
    godzillaStreamAll(ss, std::forward<Args>(args)...);
}

void godzillaMsgRaw(const std::string & msg);

std::string
godzillaMsgFmt(const std::string & msg, const std::string & title, const std::string & color);

void godzillaErrorRaw(std::string msg, bool call_stack = false);

/// Terminate the run
[[noreturn]] void terminate();

} // namespace internal

class App;
class Object;

/// Interface for printing on terminal
///
class PrintInterface {
public:
    PrintInterface(const App & app);
    PrintInterface(const Object * obj);

protected:
    /// Print a message on a terminal
    template <typename... Args>
    void
    godzillaPrint(unsigned int level, Args &&... args) const
    {
        if (level <= this->verbosity_level) {
            std::ostringstream oss;
            internal::godzillaStreamAll(oss, this->prefix, std::forward<Args>(args)...);
            internal::godzillaMsgRaw(oss.str());
        }
    }

    /// Emit an error message with the given stringified, concatenated args and
    /// terminate the application.
    template <typename... Args>
    [[noreturn]] void
    godzillaError(Args &&... args) const
    {
        std::ostringstream oss;
        internal::godzillaStreamAll(oss, this->prefix, std::forward<Args>(args)...);
        internal::godzillaErrorRaw(oss.str());
        internal::terminate();
    }

    template <typename... Args>
    [[noreturn]] void
    godzillaErrorWithCallStack(Args &&... args) const
    {
        std::ostringstream oss;
        internal::godzillaStreamAll(oss, this->prefix, std::forward<Args>(args)...);
        internal::godzillaErrorRaw(oss.str(), true);
        internal::terminate();
    }

private:
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
    ///
    const MPI_Comm & pi_comm;
};

template <typename... Args>
[[noreturn]] void
error(Args &&... args)
{
    std::ostringstream oss;
    internal::godzillaStreamAll(oss, std::forward<Args>(args)...);
    internal::godzillaErrorRaw(oss.str());
    internal::terminate();
}

/// Check PETSc error
///
/// @param ierr Error code returned by PETSc
void checkPetscError(PetscErrorCode ierr);

} // namespace godzilla
