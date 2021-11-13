#include "Godzilla.h"
#include "DirichletBC.h"

namespace godzilla {

registerObject(DirichletBC);

InputParameters
DirichletBC::validParams()
{
    InputParameters params = BoundaryCondition::validParams();
    params += FunctionInterface::validParams();
    return params;
}

DirichletBC::DirichletBC(const InputParameters & params) :
    BoundaryCondition(params),
    FunctionInterface(params)
{
    _F_;
}

PetscInt
DirichletBC::getNumComponents() const
{
    _F_;
    return this->num_comps;
}

std::vector<DMBoundaryConditionType>
DirichletBC::getBcType() const
{
    _F_;
    std::vector<DMBoundaryConditionType> bc_type;
    bc_type.resize(this->num_comps);
    for (unsigned int i = 0; i < this->num_comps; i++)
        bc_type[i] = DM_BC_ESSENTIAL;
    return bc_type;
}

void
DirichletBC::evaluate(PetscInt dim,
                      PetscReal time,
                      const PetscReal x[],
                      PetscInt Nc,
                      PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < Nc; i++)
        u[i] = evaluateFunction(i, dim, time, x);
}

} // namespace godzilla
