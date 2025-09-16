// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "fmt/format.h"
#include <exception>
#include <string>
#include <vector>

namespace godzilla {

/// Exception thrown from our code, so that application code can determine where problems originated
/// from
class Exception : public std::exception {
public:
    template <typename... T>
    Exception(fmt::format_string<T...> format, T... args)
    {
        this->msg = fmt::format(format, std::forward<T>(args)...);
        store_call_stack();
    }

    /// Get the exception message
    const char * what() const noexcept override;

    /// Get the call stack from the time the exception occured
    const std::vector<std::string> & get_call_stack() const;

private:
    /// Store call stack
    void store_call_stack();

    /// Error message
    std::string msg;
    /// Call stack at the time exception occured
    std::vector<std::string> call_stack;
};

/// Exception for internal errors
class InternalError : public Exception {
public:
    template <typename... T>
    InternalError(fmt::format_string<T...> fmt, T... args) :
        Exception("Internal error: {}", fmt::format(fmt, std::forward<T>(args)...))
    {
    }
};

/// Exception for "not implemented"
class NotImplementedException : public Exception {
public:
    template <typename... T>
    NotImplementedException() : Exception("Not implemented")
    {
    }

    template <typename... T>
    NotImplementedException(fmt::format_string<T...> fmt, T... args) : Exception(fmt, args...)
    {
    }
};

} // namespace godzilla
