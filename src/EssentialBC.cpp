#include "EssentialBC.h"
#include "CallStack.h"
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
    const auto & components = get_components();
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_ESSENTIAL,
                                   get_name().c_str(),
                                   this->label,
                                   this->ids.size(),
                                   this->ids.data(),
                                   this->fid,
                                   components.size(),
                                   components.size() == 0 ? nullptr : components.data(),
                                   (void (*)()) get_function(),
                                   (void (*)()) get_function_t(),
                                   (void *) get_context(),
                                   nullptr));
}

} // namespace godzilla
