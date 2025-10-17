// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
ConstantAuxiliaryField::parameters()
{
    auto params = AuxiliaryField::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantAuxiliaryField::ConstantAuxiliaryField(const Parameters & pars) :
    AuxiliaryField(pars),
    values(pars.get<std::vector<Real>>("value"))
{
    CALL_STACK_MSG();
    if (this->values.empty())
        log_error("No values provided");
}

Int
ConstantAuxiliaryField::get_num_components() const
{
    CALL_STACK_MSG();
    return this->values.size();
}

void
ConstantAuxiliaryField::evaluate(Real, const Real[], Scalar u[])
{
    CALL_STACK_MSG();
    for (std::size_t c = 0; c < this->values.size(); ++c)
        u[c] = this->values[c];
}

} // namespace godzilla
