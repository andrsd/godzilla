#include "Godzilla.h"
#include "BoundaryCondition.h"
#include <assert.h>

namespace godzilla {

PetscErrorCode
__boundary_condition_function(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx)
{
    _F_;
    BoundaryCondition * bc = static_cast<BoundaryCondition *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(dim, time, x, Nc, u);
    return 0;
}


InputParameters
BoundaryCondition::validParams()
{
    InputParameters params = Object::validParams();
    params.addRequiredParam<std::vector<std::string>>("boundary", "Boundary names");
    return params;
}

BoundaryCondition::BoundaryCondition(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    boundary(getParam<std::vector<std::string>>("boundary"))
{
    _F_;
}

const std::vector<std::string> &
BoundaryCondition::getBoundary() const
{
    return this->boundary;
}

PetscInt
BoundaryCondition::getFieldId() const
{
    _F_;
    // FIXME: when we have support for multiple-fields, this needs to tell us where it belongs
    return 0;
}

} // godzilla
