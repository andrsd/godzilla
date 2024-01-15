// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
}

void
EssentialBC::create()
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
    return this->fid;
}

PetscFunc *
EssentialBC::get_function()
{
    CALL_STACK_MSG();
    return essential_boundary_condition_function;
}

PetscFunc *
EssentialBC::get_function_t()
{
    CALL_STACK_MSG();
    return essential_boundary_condition_function_t;
}

const void *
EssentialBC::get_context() const
{
    CALL_STACK_MSG();
    return this;
}

void
EssentialBC::set_up()
{
    CALL_STACK_MSG();
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
