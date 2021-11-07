#include "Godzilla.h"
#include "ConstantIC.h"
#include "CallStack.h"

namespace godzilla {

registerObject(ConstantIC);

InputParameters
ConstantIC::validParams()
{
    InputParameters params = InitialCondition::validParams();
    params.addRequiredParam<std::vector<PetscReal>>("value", "Constant values for each field component");
    return params;
}

ConstantIC::ConstantIC(const InputParameters & params) :
    InitialCondition(params),
    values(getParam<std::vector<PetscReal>>("value"))
{
    _F_;
}

PetscInt
ConstantIC::getNumComponents() const
{
    _F_;
    return this->values.size();
}

void
ConstantIC::evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < Nc; i++)
        u[i] = this->values[i];
}

}
