#include "Godzilla.h"
#include "DirichletBC.h"

namespace godzilla {

registerObject(DirichletBC);

InputParameters
DirichletBC::validParams()
{
    InputParameters params = EssentialBC::validParams();
    params += FunctionInterface::validParams();
    return params;
}

DirichletBC::DirichletBC(const InputParameters & params) :
    EssentialBC(params),
    FunctionInterface(params)
{
    _F_;
}

void
DirichletBC::create()
{
    _F_;
    FunctionInterface::create();
}

PetscInt
DirichletBC::getFieldID() const
{
    _F_;
    return 0;
}

PetscInt
DirichletBC::getNumComponents() const
{
    _F_;
    return this->num_comps;
}

std::vector<PetscInt>
DirichletBC::getComponents() const
{
    PetscInt nc = getNumComponents();
    std::vector<PetscInt> comps(nc);
    for (PetscInt i = 0; i < nc; i++)
        comps[i] = i;
    return comps;
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
