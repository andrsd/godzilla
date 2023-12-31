// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Factory.h"
#include "godzilla/CallStack.h"

namespace godzilla {

void
Factory::destroy()
{
    CALL_STACK_MSG();
    while (!objects.empty()) {
        delete objects.front();
        objects.pop_front();
    }
    while (!params.empty()) {
        delete params.front();
        params.pop_front();
    }
}

} // namespace godzilla
