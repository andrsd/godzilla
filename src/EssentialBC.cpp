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

InputParameters
EssentialBC::valid_params()
{
    InputParameters params = BoundaryCondition::valid_params();
    return params;
}

EssentialBC::EssentialBC(const InputParameters & params) : BoundaryCondition(params)
{
    _F_;
}

DMBoundaryConditionType
EssentialBC::get_bc_type() const
{
    _F_;
    return DM_BC_ESSENTIAL;
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

} // namespace godzilla
