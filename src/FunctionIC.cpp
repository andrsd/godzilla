#include "Godzilla.h"
#include "CallStack.h"
#include "FunctionIC.h"

namespace godzilla {

registerObject(FunctionIC);

InputParameters
FunctionIC::valid_params()
{
    InputParameters params = InitialCondition::valid_params();
    params += FunctionInterface::valid_params();
    return params;
}

FunctionIC::FunctionIC(const InputParameters & params) :
    InitialCondition(params),
    FunctionInterface(params)
{
}

void
FunctionIC::create()
{
    _F_;
    FunctionInterface::create();
}

PetscInt
FunctionIC::get_num_components() const
{
    return this->num_comps;
}

void
FunctionIC::evaluate(PetscInt dim,
                     PetscReal time,
                     const PetscReal x[],
                     PetscInt Nc,
                     PetscScalar u[])
{
    for (PetscInt i = 0; i < Nc; i++)
        u[i] = FunctionInterface::evaluate(i, dim, time, x);
}

} // namespace godzilla
