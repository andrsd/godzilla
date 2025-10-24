// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/App.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/BoundaryCondition.h"

namespace godzilla {

Parameters
BoundaryCondition::parameters()
{
    auto params = Object::parameters();
    params.add_required_param<std::vector<std::string>>("boundary", "Boundary name")
        .add_private_param<DiscreteProblemInterface *>("_dpi");
    return params;
}

BoundaryCondition::BoundaryCondition(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    dpi(pars.get<DiscreteProblemInterface *>("_dpi")),
    boundary(pars.get<std::vector<std::string>>("boundary"))
{
    CALL_STACK_MSG();
    assert_true(this->dpi, "DiscreteProblemInterface is null");
}

Problem *
BoundaryCondition::get_problem() const
{
    CALL_STACK_MSG();
    return this->dpi->get_problem();
}

Dimension
BoundaryCondition::get_dimension() const
{
    CALL_STACK_MSG();
    return get_problem()->get_dimension();
}

const std::vector<std::string> &
BoundaryCondition::get_boundary() const
{
    CALL_STACK_MSG();
    return this->boundary;
}

DiscreteProblemInterface *
BoundaryCondition::get_discrete_problem_interface() const
{
    CALL_STACK_MSG();
    return this->dpi;
}

} // namespace godzilla
