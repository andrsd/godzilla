// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/FunctionAuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

REGISTER_OBJECT(FunctionAuxiliaryField);

static PetscErrorCode
function_auxiliary_field(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    auto * func = static_cast<FunctionAuxiliaryField *>(ctx);
    func->evaluate(dim, time, x, nc, u);
    return 0;
}

Parameters
FunctionAuxiliaryField::parameters()
{
    Parameters params = AuxiliaryField::parameters();
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

PetscFunc *
FunctionAuxiliaryField::get_func() const
{
    CALL_STACK_MSG();
    return function_auxiliary_field;
}

void
FunctionAuxiliaryField::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func(dim, time, x, nc, u);
}

} // namespace godzilla
