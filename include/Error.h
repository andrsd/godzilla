#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "petsc.h"

namespace godzilla {

namespace internal {

/// All of the following are not meant to be called directly - they are called by the normal macros
/// (godzillaError(), etc.) down below
void godzilla_stream_all(std::ostringstream & ss);

template <typename T, typename... Args>
void
godzilla_stream_all(std::ostringstream & ss, T && val, Args &&... args)
{
    ss << val;
    godzilla_stream_all(ss, std::forward<Args>(args)...);
}

void godzilla_msg_raw(const std::string & msg);

std::string
godzillaMsgFmt(const std::string & msg, const std::string & title, const std::string & color);

void godzillaErrorRaw(std::string msg, bool call_stack = false);

/// Terminate the run
[[noreturn]] void terminate(int status = 1);

void memCheck(int line, const char * func, const char * file, void * var);

} // namespace internal

template <typename... Args>
[[noreturn]] void
error(Args &&... args)
{
    std::ostringstream oss;
    internal::godzilla_stream_all(oss, std::forward<Args>(args)...);
    internal::godzillaErrorRaw(oss.str());
    internal::terminate();
}

/// Check PETSc error
///
/// @param ierr Error code returned by PETSc
void checkPetscError(PetscErrorCode ierr);

/// Check that memory allocation was ok. If not, report an error (also dump call stack) and
/// terminate
#define MEM_CHECK(var) godzilla::internal::memCheck(__LINE__, __PRETTY_FUNCTION__, __FILE__, var)

} // namespace godzilla
