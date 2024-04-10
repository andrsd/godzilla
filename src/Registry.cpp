// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Registry.h"

namespace godzilla {

bool
Registry::exists(const std::string & class_name) const
{
    auto it = this->classes.find(class_name);
    return it != this->classes.end();
}

const Registry::Entry &
Registry::get(const std::string & class_name) const
{
    auto it = this->classes.find(class_name);
    if (it == this->classes.end())
        throw Exception("Class '{}' is not registered.", class_name);
    return it->second;
}

} // namespace godzilla
