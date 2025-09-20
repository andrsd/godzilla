// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Logger.h"
#include "godzilla/CallStack.h"
#include "fmt/printf.h"

namespace godzilla {

Logger::Logger() : num_errors(0), num_warnings(0)
{
    CALL_STACK_MSG();
}

std::size_t
Logger::get_num_entries() const
{
    CALL_STACK_MSG();
    return this->entries.size();
}

std::size_t
Logger::get_num_errors() const
{
    CALL_STACK_MSG();
    return this->num_errors;
}

std::size_t
Logger::get_num_warnings() const
{
    CALL_STACK_MSG();
    return this->num_warnings;
}

void
Logger::print() const
{
    CALL_STACK_MSG();
    if (this->num_errors == 0 && this->num_warnings == 0)
        return;

    for (auto & entry : this->entries) {
        switch (entry.type) {
        case ERROR:
            fmt::print(stderr, "{}", Terminal::red);
            break;
        case WARNING:
            fmt::print(stderr, "{}", Terminal::yellow);
            break;
        }
        fmt::print(stderr, "{}", entry.text);
        fmt::print(stderr, "{}\n", Terminal::normal);
    }

    fmt::print(stderr, "{}", Terminal::magenta);
    if (this->num_errors > 0)
        fmt::print(stderr, "{} error(s)", this->num_errors);

    if (this->num_warnings > 0) {
        if (this->num_errors > 0)
            fmt::print(stderr, ", ");

        fmt::print(stderr, "{} warning(s)", this->num_warnings);
    }
    fmt::print(stderr, " found.{}\n", Terminal::normal);
}

} // namespace godzilla
