// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/DiscreteProblemInterface.h"

namespace godzilla {

Parameters
DirichletBC::parameters()
{
    auto params = EssentialBC::parameters();
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
    for (Int i = 0; i < get_num_components(); ++i)
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

void
DirichletBC::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func(time, x, get_num_components(), u);
}

void
DirichletBC::evaluate_t(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    evaluate_func_t(time, x, get_num_components(), u);
}

void
DirichletBC::set_up()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    for (auto & bnd : get_boundary())
        dpi->add_boundary_essential(get_name(),
                                    bnd,
                                    get_field_id(),
                                    get_components(),
                                    this,
                                    &DirichletBC::evaluate,
                                    has_time_expression() ? &DirichletBC::evaluate_t : nullptr);
}

} // namespace godzilla
