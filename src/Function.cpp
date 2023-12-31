// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/Function.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
Function::parameters()
{
    Parameters params = Object::parameters();
    return params;
}

Function::Function(const Parameters & params) : Object(params)
{
    CALL_STACK_MSG();
}

} // namespace godzilla
