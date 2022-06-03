#include "Godzilla.h"
#include "CallStack.h"
#include "DirichletBC.h"

namespace godzilla {

registerObject(DirichletBC);

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
    for (PetscInt i = 0; i < nc; i++)
        u[i] = FunctionInterface::evaluate(i, dim, time, x);
}

void
DirichletBC::evaluate_t(PetscInt dim,
                        PetscReal time,
                        const PetscReal x[],
                        PetscInt nc,
                        PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < nc; i++)
        u[i] = FunctionInterface::evaluate_t(i, dim, time, x);
}

void
DirichletBC::set_up_callback()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSAddBoundary(this->ds,
                              get_bc_type(),
                              get_name().c_str(),
                              this->label,
                              this->n_ids,
                              this->ids,
                              this->fid,
                              get_num_components(),
                              get_num_components() == 0 ? NULL : get_components().data(),
                              (void (*)(void)) __essential_boundary_condition_function,
                              this->evalr_t.size() > 0
                                  ? (void (*)(void)) __essential_boundary_condition_function_t
                                  : nullptr,
                              (void *) this,
                              NULL);
    check_petsc_error(ierr);
}

} // namespace godzilla
