#include "Godzilla.h"
#include "CallStack.h"
#include "DirichletBC.h"

namespace godzilla {

REGISTER_OBJECT(DirichletBC);

Parameters
DirichletBC::parameters()
{
    Parameters params = EssentialBC::parameters();
    params += FunctionInterface::parameters();
    params += FunctionInterface::valid_params_t();
    return params;
}

DirichletBC::DirichletBC(const Parameters & params) : EssentialBC(params), FunctionInterface(params)
{
    _F_;
}

void
DirichletBC::create()
{
    _F_;
    EssentialBC::create();
    FunctionInterface::create();
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

PetscFunc *
DirichletBC::get_function_t()
{
    if (!this->expression_t.empty())
        return EssentialBC::get_function_t();
    else
        return nullptr;
}

void
DirichletBC::evaluate(PetscInt dim,
                      PetscReal time,
                      const PetscReal x[],
                      PetscInt nc,
                      PetscScalar u[])
{
    _F_;
    evaluate_func(dim, time, x, nc, u);
}

void
DirichletBC::evaluate_t(PetscInt dim,
                        PetscReal time,
                        const PetscReal x[],
                        PetscInt nc,
                        PetscScalar u[])
{
    _F_;
    evaluate_func_t(dim, time, x, nc, u);
}

} // namespace godzilla
