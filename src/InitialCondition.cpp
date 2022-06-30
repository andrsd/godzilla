#include "Godzilla.h"
#include "CallStack.h"
#include "InitialCondition.h"
#include "DiscreteProblemInterface.h"
#include <assert.h>

namespace godzilla {

static PetscErrorCode
__initial_condition_function(PetscInt dim,
                             PetscReal time,
                             const PetscReal x[],
                             PetscInt Nc,
                             PetscScalar u[],
                             void * ctx)
{
    _F_;
    InitialCondition * ic = static_cast<InitialCondition *>(ctx);
    assert(ic != nullptr);
    ic->evaluate(dim, time, x, Nc, u);
    return 0;
}

InputParameters
InitialCondition::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_param<std::string>("field", "", "Field name");
    params.add_private_param<const DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

InitialCondition::InitialCondition(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    dpi(get_param<const DiscreteProblemInterface *>("_dpi")),
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
        const std::string & field_name = get_param<std::string>("field");
        if (field_name.length() > 0) {
            if (this->dpi->has_field_by_name(field_name))
                this->fid = this->dpi->get_field_id(field_name);
            else
                log_error("Field '%s' does not exists. Typo?", field_name);
        }
        else
            log_error(
                "Use the 'field' parameter to assign this initial condition to an existing field.");
    }
}

PetscInt
InitialCondition::get_field_id() const
{
    _F_;
    return this->fid;
}

PetscFunc *
InitialCondition::get_function()
{
    _F_;
    return __initial_condition_function;
}

void *
InitialCondition::get_context()
{
    return this;
}

} // namespace godzilla
