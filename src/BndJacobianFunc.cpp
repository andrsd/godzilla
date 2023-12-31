// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/BndJacobianFunc.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/NaturalBC.h"
#include "godzilla/CallStack.h"

namespace godzilla {

BndJacobianFunc::BndJacobianFunc(const BoundaryCondition * bc) :
    JacobianFunc(dynamic_cast<FEProblemInterface *>(bc->get_discrete_problem_interface()))
{
}

const Normal &
BndJacobianFunc::get_normal() const
{
    CALL_STACK_MSG();
    return get_fe_problem()->get_normal();
}

} // namespace godzilla
