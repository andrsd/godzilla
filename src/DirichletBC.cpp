#include "Godzilla.h"
#include "CallStack.h"
#include "DirichletBC.h"

namespace godzilla {

REGISTER_OBJECT(DirichletBC);

InputParameters
DirichletBC::valid_params()
{
    InputParameters params = EssentialBC::valid_params();
    params += FunctionInterface::valid_params();
    params += FunctionInterface::valid_params_t();
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

void
DirichletBC::evaluate(PetscInt dim,
                      PetscReal time,
                      const PetscReal x[],
                      PetscInt nc,
                      PetscScalar u[])
{
    _F_;
    FunctionInterface::evaluate(dim, time, x, nc, u);
}

void
DirichletBC::evaluate_t(PetscInt dim,
                        PetscReal time,
                        const PetscReal x[],
                        PetscInt nc,
                        PetscScalar u[])
{
    _F_;
    FunctionInterface::evaluate_t(dim, time, x, nc, u);
}

void
DirichletBC::set_up_callback()
{
    _F_;
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   get_bc_type(),
                                   get_name().c_str(),
                                   this->label,
                                   this->n_ids,
                                   this->ids,
                                   this->fid,
                                   get_num_components(),
                                   get_num_components() == 0 ? NULL : get_components().data(),
                                   (void (*)(void)) get_function(),
                                   this->expression_t.size() > 0 ? (void (*)(void)) get_function_t()
                                                                 : nullptr,
                                   (void *) get_context(),
                                   NULL));
}

} // namespace godzilla
