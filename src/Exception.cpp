// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Exception.h"
#include "petscsys.h"

namespace godzilla {

void
Exception::store_call_stack()
{
    auto & cs = internal::get_callstack();
    for (int n = 0, i = cs.get_size() - 1; i >= 0; i--, n++) {
        auto m = cs.at(i);
        this->call_stack.push_back(m->msg);
    }
}

const char *
Exception::what() const noexcept
{
    return this->msg.c_str();
}

const std::vector<std::string> &
Exception::get_call_stack() const
{
    return this->call_stack;
}

} // namespace godzilla
