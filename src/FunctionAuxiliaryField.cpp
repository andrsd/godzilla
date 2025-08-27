// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/FunctionAuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

Parameters
FunctionAuxiliaryField::parameters()
{
    auto params = AuxiliaryField::parameters();
    params += FunctionInterface::parameters();
    return params;
}

FunctionAuxiliaryField::FunctionAuxiliaryField(const Parameters & params) :
    AuxiliaryField(params),
    FunctionInterface(params)
{
    CALL_STACK_MSG();
}

void
FunctionAuxiliaryField::create()
{
    CALL_STACK_MSG();
    AuxiliaryField::create();
    FunctionInterface::create();
}

Int
FunctionAuxiliaryField::get_num_components() const
{
    CALL_STACK_MSG();
    return FunctionInterface::get_num_components();
}

void
FunctionAuxiliaryField::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func(time, x, get_num_components(), u);
}

} // namespace godzilla
