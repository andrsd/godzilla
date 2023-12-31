// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <exception>
#include <string>
#include "fmt/format.h"

namespace godzilla {

/// Exception thrown from our code, so that application code can determine where problems originated
/// from
class Exception : public std::exception {
public:
    template <typename... T>
    Exception(fmt::format_string<T...> format, T... args)
    {
        this->msg = fmt::format(format, std::forward<T>(args)...);
    }

    const char * what() const noexcept override;

private:
    /// Error message
    std::string msg;
};

} // namespace godzilla
