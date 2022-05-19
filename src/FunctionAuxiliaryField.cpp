#include "Godzilla.h"
#include "FunctionAuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"

namespace godzilla {

registerObject(FunctionAuxiliaryField);

PetscErrorCode
__function_auxiliary_field(PetscInt dim,
                           PetscReal time,
                           const PetscReal x[],
                           PetscInt nc,
                           PetscScalar u[],
                           void * ctx)
{
    FunctionAuxiliaryField * func = (FunctionAuxiliaryField *) ctx;
    func->evaluate(dim, time, x, nc, u);
    return 0;
}

InputParameters
FunctionAuxiliaryField::valid_params()
{
    InputParameters params = AuxiliaryField::valid_params();
    params += FunctionInterface::valid_params();
    return params;
}

FunctionAuxiliaryField::FunctionAuxiliaryField(const InputParameters & params) :
    AuxiliaryField(params),
    FunctionInterface(params)
{
    _F_;
}

void
FunctionAuxiliaryField::create()
{
    _F_;
    AuxiliaryField::create();
    FunctionInterface::create();
}

PetscInt
FunctionAuxiliaryField::get_num_components() const
{
    return this->num_comps;
}

PetscFunc *
FunctionAuxiliaryField::get_func() const
{
    return __function_auxiliary_field;
}

void
FunctionAuxiliaryField::evaluate(PetscInt dim,
                                 PetscReal time,
                                 const PetscReal x[],
                                 PetscInt nc,
                                 PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < nc; i++)
        u[i] = FunctionInterface::evaluate(i, dim, time, x);
}

} // namespace godzilla
