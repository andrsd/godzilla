#include "Godzilla.h"
#include "FunctionIC.h"

namespace godzilla {

registerObject(FunctionIC);

InputParameters
FunctionIC::validParams()
{
    InputParameters params = InitialCondition::validParams();
    params += FunctionInterface::validParams();
    return params;
}

FunctionIC::FunctionIC(const InputParameters & params) :
    InitialCondition(params),
    FunctionInterface(params)
{
}

PetscInt
FunctionIC::getNumComponents() const
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
        u[i] = evaluateFunction(i, dim, time, x);
}

} // namespace godzilla
