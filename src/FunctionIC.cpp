#include "Godzilla.h"
#include "CallStack.h"
#include "FunctionIC.h"

namespace godzilla {

REGISTER_OBJECT(FunctionIC);

Parameters
FunctionIC::parameters()
{
    Parameters params = InitialCondition::parameters();
    params += FunctionInterface::parameters();
    return params;
}

FunctionIC::FunctionIC(const Parameters & params) :
    InitialCondition(params),
    FunctionInterface(params)
{
}

void
FunctionIC::create()
{
    _F_;
    InitialCondition::create();
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
                     PetscInt nc,
                     PetscScalar u[])
{
    evaluate_func(dim, time, x, nc, u);
}

} // namespace godzilla
