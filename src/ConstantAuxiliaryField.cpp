#include "godzilla/Godzilla.h"
#include "godzilla/ConstantAuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"
#include <cassert>

namespace godzilla {

REGISTER_OBJECT(ConstantAuxiliaryField);

static PetscErrorCode
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
    _F_;
    assert(this->values.size() >= 1);
}

void
ConstantAuxiliaryField::create()
{
    _F_;
    AuxiliaryField::create();
}

Int
ConstantAuxiliaryField::get_num_components() const
{
    _F_;
    return this->values.size();
}

PetscFunc *
ConstantAuxiliaryField::get_func() const
{
    _F_;
    return constant_auxiliary_field;
}

void
ConstantAuxiliaryField::evaluate(Int, Real, const Real[], Int nc, Scalar u[])
{
    _F_;
    for (Int c = 0; c < nc; c++)
        u[c] = this->values[c];
}

} // namespace godzilla
