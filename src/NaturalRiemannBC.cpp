// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NaturalRiemannBC.h"
#include "godzilla/CallStack.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Assert.h"

namespace godzilla {

Parameters
NaturalRiemannBC::parameters()
{
    auto params = BoundaryCondition::parameters();
    return params;
}

NaturalRiemannBC::NaturalRiemannBC(const Parameters & params) :
    BoundaryCondition(params),
    fid(FieldID::INVALID)
{
    CALL_STACK_MSG();
}

void
NaturalRiemannBC::create()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    assert_true(dpi != nullptr, "DiscreteProblemInterface is null");

    auto field_names = dpi->get_field_names();
    if (field_names.size() == 1)
        this->fid = dpi->get_field_id(field_names[0]);
}

FieldID
NaturalRiemannBC::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

void
NaturalRiemannBC::set_up()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    for (auto & bnd : get_boundary())
        dpi->add_boundary_natural_riemann(get_name(),
                                          bnd,
                                          get_field_id(),
                                          get_components(),
                                          this,
                                          &NaturalRiemannBC::evaluate);
}

} // namespace godzilla
