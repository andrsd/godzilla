#include "NaturalRiemannBC.h"
#include "CallStack.h"
#include "DiscreteProblemInterface.h"
#include "UnstructuredMesh.h"
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
NaturalRiemannBC::set_up()
{
    _F_;
    const UnstructuredMesh * mesh = this->dpi->get_mesh();
    auto label = mesh->get_face_set_label(this->boundary);
    IndexSet is = IndexSet::values_from_label(label);
    is.get_indices();
    auto ids = is.to_std_vector();
    this->dpi->add_boundary_natural_riemann(get_name(),
                                            label,
                                            ids,
                                            this->fid,
                                            get_components(),
                                            natural_riemann_boundary_condition_function,
                                            nullptr,
                                            this);
    is.restore_indices();
    is.destroy();
}

} // namespace godzilla
