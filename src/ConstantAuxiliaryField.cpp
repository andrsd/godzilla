// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"
#include <cassert>

namespace godzilla {

Parameters
ConstantAuxiliaryField::parameters()
{
    auto params = AuxiliaryField::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantAuxiliaryField::ConstantAuxiliaryField(const Parameters & params) :
    AuxiliaryField(params),
    values(params.get<std::vector<Real>>("value"))
{
    CALL_STACK_MSG();
    assert(!this->values.empty());
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
