#include "EssentialBC.h"
#include "CallStack.h"
#include "UnstructuredMesh.h"
#include "DiscreteProblemInterface.h"
#include <cassert>

namespace godzilla {

static PetscErrorCode
essential_boundary_condition_function(Int dim,
                                      Real time,
                                      const Real x[],
                                      Int nc,
                                      Scalar u[],
                                      void * ctx)
{
    _F_;
    auto * bc = static_cast<EssentialBC *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(dim, time, x, nc, u);
    return 0;
}

static PetscErrorCode
essential_boundary_condition_function_t(Int dim,
                                        Real time,
                                        const Real x[],
                                        Int nc,
                                        Scalar u[],
                                        void * ctx)
{
    _F_;
    auto * bc = static_cast<EssentialBC *>(ctx);
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
    return essential_boundary_condition_function;
}

PetscFunc *
EssentialBC::get_function_t()
{
    _F_;
    return essential_boundary_condition_function_t;
}

void *
EssentialBC::get_context()
{
    _F_;
    return this;
}

void
EssentialBC::set_up()
{
    _F_;
    const UnstructuredMesh * mesh = this->dpi->get_mesh();
    auto label = mesh->get_face_set_label(this->boundary);
    IndexSet is = IndexSet::values_from_label(label);
    is.get_indices();
    auto ids = is.to_std_vector();
    this->dpi->add_boundary_essential(get_name(),
                                      label,
                                      ids,
                                      this->fid,
                                      get_components(),
                                      get_function(),
                                      get_function_t(),
                                      this);
    is.restore_indices();
    is.destroy();
}

} // namespace godzilla
