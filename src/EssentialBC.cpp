#include "EssentialBC.h"
#include "CallStack.h"
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
EssentialBC::add_boundary()
{
    _F_;
    this->dpi->add_boundary_essential(get_name(),
                                      this->label,
                                      this->ids,
                                      this->fid,
                                      get_components(),
                                      get_function(),
                                      get_function_t(),
                                      this);
}

} // namespace godzilla
