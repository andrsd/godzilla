// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/EssentialBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"

namespace godzilla {

ErrorCode
EssentialBC::invoke_essential_bc_delegate(Int dim,
                                          Real time,
                                          const Real x[],
                                          Int nc,
                                          Scalar u[],
                                          void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<EssentialBC *>(ctx);
    method->delegate.invoke(time, x, u);
    return 0;
}

ErrorCode
EssentialBC::invoke_essential_bc_delegate_t(Int dim,
                                            Real time,
                                            const Real x[],
                                            Int nc,
                                            Scalar u[],
                                            void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<EssentialBC *>(ctx);
    method->delegate_t.invoke(time, x, u);
    return 0;
}

Parameters
EssentialBC::parameters()
{
    auto params = BoundaryCondition::parameters();
    params.add_param<std::string>("field", "Field name");
    return params;
}

EssentialBC::EssentialBC(const Parameters & pars) :
    BoundaryCondition(pars),
    fid(FieldID::INVALID),
    field_name(pars.get<Optional<std::string>>("field"))

{
    CALL_STACK_MSG();
}

void
EssentialBC::create()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    assert_true(dpi != nullptr, "DiscreteProblemInterface is null");

    auto field_names = dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = dpi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        if (this->field_name.has_value()) {
            if (dpi->has_field_by_name(this->field_name.value()))
                this->fid = dpi->get_field_id(this->field_name.value());
            else
                log_error("Field '{}' does not exists. Typo?", this->field_name.value());
        }
        else
            log_error("Use the 'field' parameter to assign this boundary condition to an existing "
                      "field.");
    }

    this->components = create_components();
    set_up_callbacks();
}

FieldID
EssentialBC::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

const std::vector<Int> &
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
                          reinterpret_cast<void (*)()>(invoke_essential_bc_delegate),
                          this->delegate_t
                              ? reinterpret_cast<void (*)()>(invoke_essential_bc_delegate_t)
                              : nullptr,
                          this);
    }
}

void
EssentialBC::evaluate(Real time, const Real x[], Scalar u[])
{
    assert_true(this->delegate, "Compute delegate not set");
    this->delegate.invoke(time, x, u);
}

void
EssentialBC::evaluate_t(Real time, const Real x[], Scalar u[])
{
    assert_true(this->delegate_t, "Compute time delegate not set");
    this->delegate_t.invoke(time, x, u);
}

} // namespace godzilla
