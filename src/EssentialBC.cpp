#include "EssentialBC.h"
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
EssentialBC::validParams()
{
    InputParameters params = BoundaryCondition::validParams();
    return params;
}

EssentialBC::EssentialBC(const InputParameters & params) : BoundaryCondition(params)
{
    _F_;
}

DMBoundaryConditionType
EssentialBC::getBcType() const
{
    _F_;
    return DM_BC_ESSENTIAL;
}

void
EssentialBC::setUpCallback(PetscDS ds, DMLabel label, PetscInt n_ids, const PetscInt ids[])
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSAddBoundary(ds,
                              getBcType(),
                              getName().c_str(),
                              label,
                              n_ids,
                              ids,
                              getFieldID(),
                              getNumComponents(),
                              getComponents().size() == 0 ? NULL : getComponents().data(),
                              (void (*)(void)) & __essential_boundary_condition_function,
                              NULL,
                              (void *) this,
                              NULL);
    checkPetscError(ierr);
}

} // namespace godzilla
