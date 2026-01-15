// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"
#include <numeric>

namespace godzilla {

PetscErrorCode
InitialCondition::invoke_delegate(Int dim,
                                  Real time,
                                  const Real x[],
                                  Int nc,
                                  Scalar u[],
                                  void * ctx)
{
    CALL_STACK_MSG();
    auto * ic = static_cast<InitialCondition *>(ctx);
    ic->evaluate(time, x, u);
    return 0;
}

Parameters
InitialCondition::parameters()
{
    auto params = Object::parameters();
    params.add_param<String>("field", "Field name")
        .add_private_param<DiscreteProblemInterface *>("_dpi");
    return params;
}

InitialCondition::InitialCondition(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    dpi(pars.get<DiscreteProblemInterface *>("_dpi")),
    field_name(pars.get<Optional<String>>("field")),
    fid(FieldID::INVALID)
{
    CALL_STACK_MSG();
}

void
InitialCondition::create()
{
    CALL_STACK_MSG();
    expect_true(this->dpi != nullptr, "DiscreteProblemInterface is null");
    if (this->field_name.has_value()) {
        auto fld_name = this->field_name.value();
        if (auto id = this->dpi->get_field_id(fld_name); id.has_value())
            this->fid = id.value();
        else if (auto id = this->dpi->get_aux_field_id(fld_name); id.has_value())
            this->fid = id.value();
        else
            error("Field '{}' does not exist. Typo?", fld_name);
    }
    else {
        auto field_names = this->dpi->get_field_names();
        auto aux_field_names = this->dpi->get_aux_field_names();
        if ((field_names.size() == 1) && (aux_field_names.empty())) {
            this->fid = this->dpi->get_field_id(field_names[0]).value();
            this->field_name = this->dpi->get_field_name(this->fid).value();
        }
        else
            throw Exception(
                "Use the 'field' parameter to assign this initial condition to an existing field.");
    }

    this->components = create_components();
}

String
InitialCondition::get_field_name() const
{
    CALL_STACK_MSG();
    expect_true(this->field_name.has_value(), "Field name not set");
    return this->field_name.value();
}

FieldID
InitialCondition::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

Int
InitialCondition::get_num_components() const
{
    CALL_STACK_MSG();
    return this->components.size();
}

Dimension
InitialCondition::get_dimension() const
{
    CALL_STACK_MSG();
    return this->dpi->get_problem()->get_dimension();
}

std::vector<Int>
InitialCondition::create_components()
{
    CALL_STACK_MSG();
    auto fld = this->field_name.value();
    Int n_comps = 0;
    if (this->dpi->has_field_by_name(fld))
        n_comps = this->dpi->get_field_num_components(this->fid).value();
    else if (this->dpi->has_aux_field_by_name(fld))
        n_comps = this->dpi->get_aux_field_num_components(this->fid).value();

    std::vector<Int> comps(n_comps);
    std::iota(comps.begin(), comps.end(), 0);
    return comps;
}

} // namespace godzilla
