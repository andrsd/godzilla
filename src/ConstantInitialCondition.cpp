// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/CallStack.h"
#include <numeric>

namespace godzilla {

Parameters
ConstantInitialCondition::parameters()
{
    auto params = InitialCondition::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantInitialCondition::ConstantInitialCondition(const Parameters & pars) :
    InitialCondition(pars),
    values(pars.get<std::vector<Real>>("value"))
{
    CALL_STACK_MSG();
}

std::vector<Int>
ConstantInitialCondition::create_components()
{
    CALL_STACK_MSG();
    std::vector<Int> comps(this->values.size());
    std::iota(comps.begin(), comps.end(), 0);
    return comps;
}

void
ConstantInitialCondition::evaluate(Real time, const Real x[], Scalar u[])
{
    CALL_STACK_MSG();
    for (std::size_t i = 0; i < this->values.size(); ++i)
        u[i] = this->values[i];
}

} // namespace godzilla
