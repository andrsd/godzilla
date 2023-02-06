#include "NaturalRiemannBC.h"
#include "CallStack.h"
#include <cassert>

namespace godzilla {

static PetscErrorCode
natural_riemann_boundary_condition_function(Real time,
                                            const Real * c,
                                            const Real * n,
                                            const Scalar * xI,
                                            Scalar * xG,
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
    const auto & components = get_components();
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_NATURAL_RIEMANN,
                                   get_name().c_str(),
                                   this->label,
                                   this->ids.size(),
                                   this->ids.data(),
                                   this->fid,
                                   components.size(),
                                   components.size() == 0 ? nullptr : components.data(),
                                   (void (*)()) natural_riemann_boundary_condition_function,
                                   nullptr,
                                   (void *) this,
                                   &this->bd));
}

} // namespace godzilla
