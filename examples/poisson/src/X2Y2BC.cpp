#include "Godzilla.h"
#include "X2Y2BC.h"

namespace godzilla {

registerObject(X2Y2BC);

InputParameters
X2Y2BC::validParams()
{
    InputParameters params = BoundaryCondition::validParams();
    return params;
}

X2Y2BC::X2Y2BC(const InputParameters & params) : BoundaryCondition(params)
{
    _F_;
}

PetscInt
X2Y2BC::getNumComponents() const
{
    _F_;
    return 1;
}

std::vector<DMBoundaryConditionType>
X2Y2BC::getBcType() const
{
    _F_;
    return { DM_BC_ESSENTIAL };
}

void
X2Y2BC::evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[])
{
    _F_;
    *u = x[0] * x[0] + x[1] * x[1];
}

} // namespace godzilla
