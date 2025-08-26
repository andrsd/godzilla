// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/FunctionInitialCondition.h"

namespace godzilla {

Parameters
FunctionInitialCondition::parameters()
{
    auto params = InitialCondition::parameters();
    params += FunctionInterface::parameters();
    return params;
}

FunctionInitialCondition::FunctionInitialCondition(const Parameters & params) :
    InitialCondition(params),
    FunctionInterface(params)
{
}

void
FunctionInitialCondition::create()
{
    CALL_STACK_MSG();
    InitialCondition::create();
    FunctionInterface::create();
}

Int
FunctionInitialCondition::get_num_components() const
{
    CALL_STACK_MSG();
    return FunctionInterface::get_num_components();
}

void
FunctionInitialCondition::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func(time, x, get_num_components(), u);
}

} // namespace godzilla
