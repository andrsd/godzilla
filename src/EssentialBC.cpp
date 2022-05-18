#include "EssentialBC.h"
#include "CallStack.h"
#include <assert.h>

namespace godzilla {

PetscErrorCode
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

void
EssentialBC::set_up_callback()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSAddBoundary(this->ds,
                              get_bc_type(),
                              get_name().c_str(),
                              this->label,
                              this->n_ids,
                              this->ids,
                              get_field_id(),
                              get_num_components(),
                              get_num_components() == 0 ? NULL : get_components().data(),
                              (void (*)(void)) & __essential_boundary_condition_function,
                              NULL,
                              (void *) this,
                              NULL);
    check_petsc_error(ierr);
}

} // namespace godzilla
