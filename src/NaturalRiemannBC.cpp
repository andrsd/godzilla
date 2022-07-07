#include "NaturalRiemannBC.h"
#include "CallStack.h"
#include <assert.h>

namespace godzilla {

static PetscErrorCode
__natural_riemann_boundary_condition_function(PetscReal time,
                                              const PetscReal * c,
                                              const PetscReal * n,
                                              const PetscScalar * xI,
                                              PetscScalar * xG,
                                              void * ctx)
{
    _F_;
    NaturalRiemannBC * bc = static_cast<NaturalRiemannBC *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(time, c, n, xI, xG);
    return 0;
}

Parameters
NaturalRiemannBC::valid_params()
{
    Parameters params = BoundaryCondition::valid_params();
    return params;
}

NaturalRiemannBC::NaturalRiemannBC(const Parameters & params) : BoundaryCondition(params), bd(-1)
{
    _F_;
}

DMBoundaryConditionType
NaturalRiemannBC::get_bc_type() const
{
    _F_;
    return DM_BC_NATURAL_RIEMANN;
}

void
NaturalRiemannBC::set_up_callback()
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
                                   get_num_components() == 0 ? nullptr : get_components().data(),
                                   (void (*)(void)) __natural_riemann_boundary_condition_function,
                                   nullptr,
                                   (void *) this,
                                   &this->bd));
}

} // namespace godzilla
