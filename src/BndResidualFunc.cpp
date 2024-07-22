// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/BndResidualFunc.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/CallStack.h"

namespace godzilla {

AbstractBndResidualFunc::AbstractBndResidualFunc(const BoundaryCondition * bc) :
    AbstractResidualFunctional(
        dynamic_cast<FEProblemInterface *>(bc->get_discrete_problem_interface()))
{
}

const Normal &
AbstractBndResidualFunc::get_normal() const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_normal();
}

} // namespace godzilla
