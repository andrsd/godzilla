// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
ConstantInitialCondition::parameters()
{
    Parameters params = InitialCondition::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantInitialCondition::ConstantInitialCondition(const Parameters & params) :
    InitialCondition(params),
    values(get_param<std::vector<Real>>("value"))
{
    CALL_STACK_MSG();
}

Int
ConstantInitialCondition::get_num_components() const
{
    CALL_STACK_MSG();
    return (Int) this->values.size();
}

void
ConstantInitialCondition::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    for (Int i = 0; i < this->values.size(); i++)
        u[i] = this->values[i];
}

} // namespace godzilla
