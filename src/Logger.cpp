// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Logger.h"
#include "godzilla/CallStack.h"
#include "fmt/printf.h"

namespace godzilla {

Logger::Logger() : num_errors(0), num_warnings(0)
{
    _F_;
}

std::size_t
Logger::get_num_entries() const
{
    _F_;
    return this->entries.size();
}

std::size_t
Logger::get_num_errors() const
{
    _F_;
    return this->num_errors;
}

std::size_t
Logger::get_num_warnings() const
{
    _F_;
    return this->num_warnings;
}

void
Logger::print() const
{
    _F_;
    for (auto & entry : this->entries) {
        switch (entry.type) {
        case ERROR:
            fmt::print(stderr, "{}", Terminal::Color::red);
            break;
        case WARNING:
            fmt::print(stderr, "{}", Terminal::Color::yellow);
            break;
        }
        fmt::print(stderr, entry.text);
        fmt::print(stderr, "{}\n", Terminal::Color::normal);
    }

    fmt::print(stderr, "{}", Terminal::Color::magenta);
    if (this->num_errors > 0)
        fmt::print(stderr, "{} error(s)", this->num_errors);

    if (this->num_warnings > 0) {
        if (this->num_errors > 0)
            fmt::print(stderr, ", ");

        fmt::print(stderr, "{} warning(s)", this->num_warnings);
    }
    fmt::print(stderr, " found.{}\n", Terminal::Color::normal);
}

} // namespace godzilla
