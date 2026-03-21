// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include <exception>
#include <vector>

namespace godzilla {

/// Exception thrown from our code, so that application code can determine where problems originated
/// from
class Exception : public std::exception {
public:
    Exception(const std::string & msg,
              const std::source_location location = std::source_location::current());

    Exception(int rank,
              const std::string & msg,
              const std::source_location location = std::source_location::current());

    /// Get the exception message
    const char * what() const noexcept override;

    /// Get location where the exception occured
    const std::source_location location() const;

    /// Print the call stack from the time the exception occured
    void print_stack() const;

    int rank() const;

private:
    /// Store call stack
    void store_call_stack();

    /// Rank of the process that threw the exception
    int mpi_rank = -1;
    /// Error message
    std::string msg;
    /// Location where the exception occured
    std::source_location loc;
    /// Call stack at the time exception occured
    std::vector<CallStack::Frame> call_stack;
};

/// Exception for internal errors
class InternalError : public Exception {
public:
    InternalError(const std::string & msg,
                  const std::source_location location = std::source_location::current()) :
        Exception(msg, location)
    {
    }
};

/// Exception for "not implemented"
class NotImplementedException : public Exception {
public:
    NotImplementedException(const std::source_location location = std::source_location::current()) :
        Exception("", location)
    {
    }

    NotImplementedException(const std::string & msg,
                            const std::source_location location = std::source_location::current()) :
        Exception(msg, location)
    {
    }
};

/// Print an exception to the console
void print(Exception & e);
void print(InternalError & e);
void print(NotImplementedException & e);

} // namespace godzilla
