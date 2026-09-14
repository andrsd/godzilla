// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include "godzilla/Range.h"
#include <fmt/core.h>
#include <ranges>

namespace godzilla {

Exception::Exception(const std::string & msg, const std::source_location location) :
    msg_(msg),
    loc_(location)
{
    store_call_stack();
}

Exception::Exception(int rank, const std::string & msg, const std::source_location location) :
    mpi_rank_(rank),
    msg_(msg),
    loc_(location)
{
    store_call_stack();
}

void
Exception::store_call_stack()
{
    auto & call_stack = get_callstack();
    for (auto idx : make_range(call_stack.get_size())) {
        auto frame = call_stack[idx];
        this->call_stack_.push_back(*frame);
    }
}

const char *
Exception::what() const noexcept
{
    return this->msg_.c_str();
}

const std::source_location
Exception::location() const
{
    return this->loc_;
}

int
Exception::rank() const
{
    return this->mpi_rank_;
}

void
Exception::print_stack() const
{
    fmt::println(stderr, "  at {} ({}:{})", loc_.function_name(), loc_.file_name(), loc_.line());
    for (auto & m : this->call_stack_ | std::views::reverse)
        if (loc_.function_name() != m.function)
            fmt::println(stderr, "  at {} ({}:{})", m.function, m.file, m.line);
}

void
print(Exception & e)
{
    if (e.rank() != -1)
        fmt::println(stderr, "[{}] {}", e.rank(), e.what());
    else
        fmt::println(stderr, "{}", e.what());
    e.print_stack();
}

void
print(InternalError & e)
{
    if (e.rank() != -1)
        fmt::println(stderr, "[{}] InternalError: {}", e.rank(), e.what());
    else
        fmt::println(stderr, "InternalError: {}", e.what());
    e.print_stack();
}

void
print(NotImplementedException & e)
{
    if (e.rank() != -1)
        fmt::println(stderr, "[{}] NotImplementedException: {}", e.rank(), e.what());
    else
        fmt::println(stderr, "NotImplementedException: {}", e.what());
    e.print_stack();
}

} // namespace godzilla
