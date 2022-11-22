#include "EssentialBC.h"
#include "CallStack.h"
#include <assert.h>

namespace godzilla {

static PetscErrorCode
__essential_boundary_condition_function(PetscInt dim,
                                        PetscReal time,
                                        const PetscReal x[],
                                        PetscInt nc,
                                        PetscScalar u[],
                                        void * ctx)
{
    _F_;
    EssentialBC * bc = static_cast<EssentialBC *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(dim, time, x, nc, u);
    return 0;
}

static PetscErrorCode
__essential_boundary_condition_function_t(PetscInt dim,
                                          PetscReal time,
                                          const PetscReal x[],
                                          PetscInt nc,
                                          PetscScalar u[],
                                          void * ctx)
{
    _F_;
    EssentialBC * bc = static_cast<EssentialBC *>(ctx);
    assert(bc != nullptr);
    bc->evaluate_t(dim, time, x, nc, u);
    return 0;
}

Parameters
EssentialBC::parameters()
{
    Parameters params = BoundaryCondition::parameters();
    return params;
}

EssentialBC::EssentialBC(const Parameters & params) : BoundaryCondition(params)
{
    _F_;
}

PetscFunc *
EssentialBC::get_function()
{
    _F_;
    return __essential_boundary_condition_function;
}

PetscFunc *
EssentialBC::get_function_t()
{
    _F_;
    return __essential_boundary_condition_function_t;
}

void *
EssentialBC::get_context()
{
    _F_;
    return this;
}

void
EssentialBC::add_boundary()
{
    _F_;
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_ESSENTIAL,
                                   get_name().c_str(),
                                   this->label,
                                   this->n_ids,
                                   this->ids,
                                   this->fid,
                                   get_num_components(),
                                   get_num_components() == 0 ? nullptr : get_components().data(),
                                   (void (*)(void)) get_function(),
                                   (void (*)(void)) get_function_t(),
                                   (void *) get_context(),
                                   nullptr));
}

} // namespace godzilla
