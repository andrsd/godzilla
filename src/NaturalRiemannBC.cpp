#include "godzilla/NaturalRiemannBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/App.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
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
NaturalRiemannBC::create()
{
    _F_;
    assert(this->dpi != nullptr);

    std::vector<std::string> field_names = this->dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = this->dpi->get_field_id(field_names[0]);
    }
}

Int
NaturalRiemannBC::get_field_id() const
{
    _F_;
    return this->fid;
}

void
NaturalRiemannBC::set_up()
{
    _F_;
    for (auto & bnd : get_boundary())
        this->dpi->add_boundary_natural_riemann(get_name(),
                                                bnd,
                                                get_field_id(),
                                                get_components(),
                                                natural_riemann_boundary_condition_function,
                                                nullptr,
                                                this);
}

} // namespace godzilla
