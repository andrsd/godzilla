// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/DiscreteProblemInterface.h"
#include <cassert>

namespace godzilla {

static PetscErrorCode
initial_condition_function(Int dim, Real time, const Real x[], Int Nc, Scalar u[], void * ctx)
{
    CALL_STACK_MSG();
    auto * ic = static_cast<InitialCondition *>(ctx);
    assert(ic != nullptr);
    ic->evaluate(dim, time, x, Nc, u);
    return 0;
}

Parameters
InitialCondition::parameters()
{
    Parameters params = Object::parameters();
    params.add_param<std::string>("field", "", "Field name");
    params.add_private_param<DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

InitialCondition::InitialCondition(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    dpi(get_param<DiscreteProblemInterface *>("_dpi")),
    fid(-1)
{
    CALL_STACK_MSG();
}

void
InitialCondition::create()
{
    CALL_STACK_MSG();
    assert(this->dpi != nullptr);
    auto fld = get_param<std::string>("field");
    if (fld.length() > 0) {
        this->field_name = fld;
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
        if ((field_names.size() == 1) && (aux_field_names.size() == 0)) {
            this->fid = this->dpi->get_field_id(field_names[0]);
            this->field_name = this->dpi->get_field_name(this->fid);
        }
        else
            log_error(
                "Use the 'field' parameter to assign this initial condition to an existing field.");
    }
}

const std::string &
InitialCondition::get_field_name() const
{
    CALL_STACK_MSG();
    return this->field_name;
}

Int
InitialCondition::get_field_id() const
{
    CALL_STACK_MSG();
    return this->fid;
}

PetscFunc *
InitialCondition::get_function()
{
    CALL_STACK_MSG();
    return initial_condition_function;
}

void *
InitialCondition::get_context()
{
    CALL_STACK_MSG();
    return this;
}

} // namespace godzilla
