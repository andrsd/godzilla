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
    params.add_required_param<std::vector<String>>("boundary", "Boundary name")
        .add_private_param<LateRef<DiscreteProblemInterface>>("_dpi");
    return params;
}

BoundaryCondition::BoundaryCondition(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    dpi_(pars.get<Ref<DiscreteProblemInterface>>("_dpi")),
    boundary_(pars.get<std::vector<String>>("boundary"))
{
    CALL_STACK_MSG();
}

Ref<Problem>
BoundaryCondition::get_problem() const
{
    CALL_STACK_MSG();
    return this->dpi_->get_problem();
}

Dimension
BoundaryCondition::get_dimension() const
{
    CALL_STACK_MSG();
    return get_problem()->get_dimension();
}

Span<const String>
BoundaryCondition::get_boundary() const
{
    CALL_STACK_MSG();
    return this->boundary_;
}

Ref<DiscreteProblemInterface>
BoundaryCondition::get_discrete_problem_interface() const
{
    CALL_STACK_MSG();
    return this->dpi_;
}

} // namespace godzilla
