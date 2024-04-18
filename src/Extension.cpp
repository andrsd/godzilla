// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Extension.h"

namespace godzilla {

Extension::Extension(const std::string & extension_name) : name(extension_name) {}

const std::string &
Extension::get_name() const
{
    return this->name;
}

} // namespace godzilla
