// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"

namespace godzilla {

Parameters
InitialCondition::parameters()
{
    auto params = Object::parameters();
    params.add_param<std::string>("field", "Field name")
        .add_private_param<DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

InitialCondition::InitialCondition(const Parameters & pars) :
    Object(pars),
    PrintInterface(this),
    dpi(pars.get<DiscreteProblemInterface *>("_dpi")),
    field_name(pars.get<Optional<std::string>>("field")),
    fid(FieldID::INVALID)
{
    CALL_STACK_MSG();
}

void
InitialCondition::create()
{
    CALL_STACK_MSG();
    assert_true(this->dpi != nullptr, "DiscreteProblemInterface is null");
    if (this->field_name.has_value()) {
        auto fld = this->field_name.value();
        if (this->dpi->has_field_by_name(fld))
            this->fid = this->dpi->get_field_id(fld);
        else if (this->dpi->has_aux_field_by_name(fld))
            this->fid = this->dpi->get_aux_field_id(fld);
        else
            log_error("Field '{}' does not exists. Typo?", fld);
    }
    else {
        std::vector<std::string> field_names = this->dpi->get_field_names();
        std::vector<std::string> aux_field_names = this->dpi->get_aux_field_names();
        if ((field_names.size() == 1) && (aux_field_names.empty())) {
            this->fid = this->dpi->get_field_id(field_names[0]);
            this->field_name = this->dpi->get_field_name(this->fid);
        }
        else
            throw Exception(
                "Use the 'field' parameter to assign this initial condition to an existing field.");
    }
}

const std::string &
InitialCondition::get_field_name() const
{
    CALL_STACK_MSG();
    assert_true(this->field_name.has_value(), "Field name not set");
    return this->field_name.value();
}

FieldID
InitialCondition::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

Dimension
InitialCondition::get_dimension() const
{
    CALL_STACK_MSG();
    return this->dpi->get_problem()->get_dimension();
}

} // namespace godzilla
