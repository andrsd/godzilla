#include "Godzilla.h"
#include "CallStack.h"
#include "InitialCondition.h"
#include <assert.h>

namespace godzilla {

PetscErrorCode
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
InitialCondition::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

InitialCondition::InitialCondition(const InputParameters & params) :
    Object(params),
    PrintInterface(this)
{
    _F_;
}

PetscInt
InitialCondition::getFieldId() const
{
    _F_;
    // FIXME: when we have support for multiple-fields, this needs to tell us where it belongs
    return 0;
}

} // namespace godzilla
