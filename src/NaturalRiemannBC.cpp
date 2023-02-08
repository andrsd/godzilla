#include "NaturalRiemannBC.h"
#include "CallStack.h"
#include "DiscreteProblemInterface.h"
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

NaturalRiemannBC::NaturalRiemannBC(const Parameters & params) : BoundaryCondition(params)
{
    _F_;
}

void
NaturalRiemannBC::add_boundary()
{
    _F_;
    this->dpi->add_boundary_natural_riemann(get_name(),
                                            this->label,
                                            this->ids,
                                            this->fid,
                                            get_components(),
                                            natural_riemann_boundary_condition_function,
                                            nullptr,
                                            this);
}

} // namespace godzilla
