#include "Godzilla.h"
#include "ConstantAuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"
#include <assert.h>

namespace godzilla {

REGISTER_OBJECT(ConstantAuxiliaryField);

PetscErrorCode
__constant_auxiliary_field(PetscInt dim,
                           PetscReal time,
                           const PetscReal x[],
                           PetscInt nc,
                           PetscScalar u[],
                           void * ctx)
{
    ConstantAuxiliaryField * aux_fld = static_cast<ConstantAuxiliaryField *>(ctx);
    aux_fld->evaluate(dim, time, x, nc, u);
    return 0;
}

Parameters
ConstantAuxiliaryField::parameters()
{
    Parameters params = AuxiliaryField::parameters();
    params.add_required_param<std::vector<PetscReal>>("value",
                                                      "Constant values for each field component");
    return params;
}

ConstantAuxiliaryField::ConstantAuxiliaryField(const Parameters & params) :
    AuxiliaryField(params),
    values(params.get<std::vector<PetscReal>>("value"))
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

PetscInt
ConstantAuxiliaryField::get_num_components() const
{
    return this->num_comps;
}

PetscFunc *
ConstantAuxiliaryField::get_func() const
{
    return __constant_auxiliary_field;
}

void
ConstantAuxiliaryField::evaluate(PetscInt dim,
                                 PetscReal time,
                                 const PetscReal x[],
                                 PetscInt nc,
                                 PetscScalar u[])
{
    _F_;
    for (PetscInt c = 0; c < nc; c++)
        u[c] = this->values[c];
}

} // namespace godzilla
