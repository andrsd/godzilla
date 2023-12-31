// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Exception.h"

namespace godzilla {

const char *
Exception::what() const noexcept
{
    return this->msg.c_str();
}

} // namespace godzilla
