// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Registry.h"

namespace godzilla {

Registry &
Registry::instance()
{
    static Registry registry;
    return registry;
}

} // namespace godzilla
