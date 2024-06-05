// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"
#include <cassert>

namespace godzilla {

static ErrorCode
constant_auxiliary_field(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    auto * aux_fld = static_cast<ConstantAuxiliaryField *>(ctx);
    aux_fld->evaluate(dim, time, x, nc, u);
    return 0;
}

Parameters
ConstantAuxiliaryField::parameters()
{
    Parameters params = AuxiliaryField::parameters();
    params.add_required_param<std::vector<Real>>("value",
                                                 "Constant values for each field component");
    return params;
}

ConstantAuxiliaryField::ConstantAuxiliaryField(const Parameters & params) :
    AuxiliaryField(params),
    values(params.get<std::vector<Real>>("value"))
{
    CALL_STACK_MSG();
    assert(this->values.size() >= 1);
}

Int
ConstantAuxiliaryField::get_num_components() const
{
    CALL_STACK_MSG();
    return this->values.size();
}

PetscFunc *
ConstantAuxiliaryField::get_func() const
{
    CALL_STACK_MSG();
    return constant_auxiliary_field;
}

void
ConstantAuxiliaryField::evaluate(Int, Real, const Real[], Int nc, Scalar u[])
{
    CALL_STACK_MSG();
    for (Int c = 0; c < nc; c++)
        u[c] = this->values[c];
}

} // namespace godzilla
