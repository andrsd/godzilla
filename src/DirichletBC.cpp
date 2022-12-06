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

DirichletBC::DirichletBC(const Parameters & params) :
    EssentialBC(params),
    FunctionInterface(params),
    components(this->num_comps, 0)
{
    _F_;
    for (PetscInt i = 0; i < this->num_comps; i++)
        this->components[i] = i;
}

void
DirichletBC::create()
{
    _F_;
    EssentialBC::create();
    FunctionInterface::create();
}

const std::vector<PetscInt> &
DirichletBC::get_components() const
{
    return this->components;
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
