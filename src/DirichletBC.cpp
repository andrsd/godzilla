#include "Godzilla.h"
#include "DirichletBC.h"

namespace godzilla {

registerObject(DirichletBC);

InputParameters
DirichletBC::valid_params()
{
    InputParameters params = EssentialBC::valid_params();
    params += FunctionInterface::valid_params();
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
DirichletBC::get_field_id() const
{
    _F_;
    return 0;
}

PetscInt
DirichletBC::get_num_components() const
{
    _F_;
    return this->num_comps;
}

std::vector<PetscInt>
DirichletBC::get_components() const
{
    PetscInt nc = get_num_components();
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
        u[i] = FunctionInterface::evaluate(i, dim, time, x);
}

} // namespace godzilla
