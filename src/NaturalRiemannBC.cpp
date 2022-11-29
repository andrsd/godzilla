#include "NaturalRiemannBC.h"
#include "CallStack.h"
#include <cassert>

namespace godzilla {

static PetscErrorCode
natural_riemann_boundary_condition_function(PetscReal time,
                                            const PetscReal * c,
                                            const PetscReal * n,
                                            const PetscScalar * xI,
                                            PetscScalar * xG,
                                            void * ctx)
{
    _F_;
    auto * bc = static_cast<NaturalRiemannBC *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(time, c, n, xI, xG);
    return 0;
}

Parameters
NaturalRiemannBC::parameters()
{
    Parameters params = BoundaryCondition::parameters();
    return params;
}

NaturalRiemannBC::NaturalRiemannBC(const Parameters & params) : BoundaryCondition(params), bd(-1)
{
    _F_;
}

void
NaturalRiemannBC::add_boundary()
{
    _F_;
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_NATURAL_RIEMANN,
                                   get_name().c_str(),
                                   this->label,
                                   this->n_ids,
                                   this->ids,
                                   this->fid,
                                   get_num_components(),
                                   get_num_components() == 0 ? nullptr : get_components().data(),
                                   (void (*)()) natural_riemann_boundary_condition_function,
                                   nullptr,
                                   (void *) this,
                                   &this->bd));
}

} // namespace godzilla
