// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Factory.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Factory::Factory(const Registry & registry) : registry(registry) {}

Factory::~Factory()
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

void
Factory::destroy()
{
}

} // namespace godzilla
