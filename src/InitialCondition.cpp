#include "Godzilla.h"
#include "CallStack.h"
#include "InitialCondition.h"
#include "DiscreteProblemInterface.h"
#include <cassert>

namespace godzilla {

static PetscErrorCode
initial_condition_function(Int dim, Real time, const Real x[], Int Nc, Scalar u[], void * ctx)
{
    _F_;
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
    _F_;
}

void
InitialCondition::create()
{
    _F_;
    assert(this->dpi != nullptr);
    std::vector<std::string> field_names = this->dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = this->dpi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        const auto & field_name = get_param<std::string>("field");
        if (field_name.length() > 0) {
            if (this->dpi->has_field_by_name(field_name))
                this->fid = this->dpi->get_field_id(field_name);
            else
                log_error("Field '{}' does not exists. Typo?", field_name);
        }
        else
            log_error(
                "Use the 'field' parameter to assign this initial condition to an existing field.");
    }
}

Int
InitialCondition::get_field_id() const
{
    _F_;
    return this->fid;
}

PetscFunc *
InitialCondition::get_function()
{
    _F_;
    return initial_condition_function;
}

void *
InitialCondition::get_context()
{
    _F_;
    return this;
}

} // namespace godzilla
