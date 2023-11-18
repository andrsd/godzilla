#include "godzilla/EssentialBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/App.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
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
    params.add_param<std::string>("field", "", "Field name");
    return params;
}

EssentialBC::EssentialBC(const Parameters & params) : BoundaryCondition(params), fid(-1)

{
    _F_;
}

void
EssentialBC::create()
{
    _F_;
    auto dpi = get_discrete_problem_interface();
    assert(dpi != nullptr);

    std::vector<std::string> field_names = dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = dpi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        const auto & field_name = get_param<std::string>("field");
        if (field_name.length() > 0) {
            if (dpi->has_field_by_name(field_name))
                this->fid = dpi->get_field_id(field_name);
            else
                log_error("Field '{}' does not exists. Typo?", field_name);
        }
        else
            log_error("Use the 'field' parameter to assign this boundary condition to an existing "
                      "field.");
    }
}

Int
EssentialBC::get_field_id() const
{
    _F_;
    return this->fid;
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
    auto dpi = get_discrete_problem_interface();
    for (auto & bnd : get_boundary())
        dpi->add_boundary_essential(get_name(),
                                    bnd,
                                    get_field_id(),
                                    get_components(),
                                    get_function(),
                                    get_function_t(),
                                    this);
}

} // namespace godzilla
