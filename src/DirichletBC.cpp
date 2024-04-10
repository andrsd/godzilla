// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/DirichletBC.h"

namespace godzilla {

Parameters
DirichletBC::parameters()
{
    Parameters params = EssentialBC::parameters();
    params += FunctionInterface::parameters();
    params += FunctionInterface::valid_params_t();
    return params;
}

DirichletBC::DirichletBC(const Parameters & params) :
    EssentialBC(params),
    FunctionInterface(params),
    components(get_num_components(), 0)
{
    CALL_STACK_MSG();
    for (Int i = 0; i < get_num_components(); i++)
        this->components[i] = i;
}

void
DirichletBC::create()
{
    CALL_STACK_MSG();
    EssentialBC::create();
    FunctionInterface::create();
}

const std::vector<Int> &
DirichletBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

PetscFunc *
DirichletBC::get_function_t()
{
    CALL_STACK_MSG();
    if (has_time_expression())
        return EssentialBC::get_function_t();
    else
        return nullptr;
}

void
DirichletBC::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func(dim, time, x, nc, u);
}

void
DirichletBC::evaluate_t(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func_t(dim, time, x, nc, u);
}

} // namespace godzilla
