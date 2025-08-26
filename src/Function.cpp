// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/Function.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/App.h"

namespace godzilla {

Parameters
Function::parameters()
{
    auto params = Object::parameters();
    return params;
}

Function::Function(const Parameters & params) : Object(params)
{
    CALL_STACK_MSG();
}

Int
Function::get_dimension() const
{
    CALL_STACK_MSG();
    return get_app()->get_problem()->get_dimension();
}

} // namespace godzilla
