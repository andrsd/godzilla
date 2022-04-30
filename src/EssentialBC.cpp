#include "EssentialBC.h"
#include <assert.h>

namespace godzilla {

InputParameters
EssentialBC::valid_params()
{
    InputParameters params = BoundaryCondition::valid_params();
    return params;
}

EssentialBC::EssentialBC(const InputParameters & params) : BoundaryCondition(params)
{
    _F_;
}

BoundaryConditionType
EssentialBC::get_bc_type() const
{
    _F_;
    return BC_ESSENTIAL;
}

} // namespace godzilla
