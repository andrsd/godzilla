#include "Godzilla.h"
#include "ConstantAuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"
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
    this->num_comps = this->values.size();
    assert(this->num_comps >= 1);
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
    return this->num_comps;
}

PetscFunc *
ConstantAuxiliaryField::get_func() const
{
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
