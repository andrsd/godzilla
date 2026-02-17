// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/EssentialBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"
#include <numeric>

namespace godzilla {

PetscErrorCode
EssentialBC::invoke_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    CALL_STACK_MSG();
    auto * bc = static_cast<EssentialBC *>(ctx);
    bc->evaluate(time, x, u);
    return 0;
}

PetscErrorCode
EssentialBC::invoke_delegate_t(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    CALL_STACK_MSG();
    auto * bc = static_cast<EssentialBC *>(ctx);
    bc->evaluate_t(time, x, u);
    return 0;
}

Parameters
EssentialBC::parameters()
{
    auto params = BoundaryCondition::parameters();
    params.add_param<String>("field", "Field name");
    return params;
}

EssentialBC::EssentialBC(const Parameters & pars) :
    BoundaryCondition(pars),
    fid(FieldID::INVALID),
    field_name(pars.get<Optional<String>>("field"))

{
    CALL_STACK_MSG();
}

void
EssentialBC::create()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();

    auto field_names = dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = dpi->get_field_id(field_names[0]).value();
    }
    else if (field_names.size() > 1) {
        expect_true(this->field_name.has_value(),
                    "Use the 'field' parameter to assign this boundary condition to an existing "
                    "field.");
        auto fld = dpi->get_field_id(this->field_name.value());
        expect_true(fld.has_value(), "Field '{}' does not exist. Typo?", this->field_name.value());
        this->fid = fld.value();
    }

    this->components = create_components();
}

FieldID
EssentialBC::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

Span<const Int>
EssentialBC::get_components() const
{
    CALL_STACK_MSG();
    return this->components;
}

void
EssentialBC::set_up()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    auto mesh = dpi->get_mesh();
    for (auto & boundary : get_boundary()) {
        Label label;
        if (mesh->has_face_set(boundary))
            label = mesh->get_face_set_label(boundary);
        else if (mesh->has_vertex_set(boundary))
            label = mesh->get_vertex_set_label(boundary);
        else
            throw Exception("Boundary '{}' does not exist.", boundary);
        auto ids = label.get_values();
        dpi->add_boundary(DM_BC_ESSENTIAL,
                          get_name(),
                          label,
                          ids,
                          this->fid,
                          this->components,
                          reinterpret_cast<void (*)()>(invoke_delegate),
                          reinterpret_cast<void (*)()>(invoke_delegate_t),
                          this);
    }
}

std::vector<Int>
EssentialBC::create_components()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    auto n_comps = dpi->get_field_num_components(this->fid);
    expect_true(n_comps.has_value(), "Field {} not found", this->fid);
    std::vector<Int> comps(n_comps.value());
    std::iota(comps.begin(), comps.end(), 0);
    return comps;
}

void
EssentialBC::evaluate_t(Real time, const Real x[], Scalar u[])
{
}

} // namespace godzilla
