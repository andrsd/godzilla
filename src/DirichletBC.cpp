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
    for (Int i = 0; i < this->num_comps; i++)
        this->components[i] = i;
}

void
DirichletBC::create()
{
    _F_;
    EssentialBC::create();
    FunctionInterface::create();
}

const std::vector<Int> &
DirichletBC::get_components() const
{
    _F_;
    return this->components;
}

PetscFunc *
DirichletBC::get_function_t()
{
    _F_;
    if (!this->expression_t.empty())
        return EssentialBC::get_function_t();
    else
        return nullptr;
}

void
DirichletBC::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    _F_;
    evaluate_func(dim, time, x, nc, u);
}

void
DirichletBC::evaluate_t(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    _F_;
    evaluate_func_t(dim, time, x, nc, u);
}

} // namespace godzilla
