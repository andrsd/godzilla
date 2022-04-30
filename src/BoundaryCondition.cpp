#include "Godzilla.h"
#include "BoundaryCondition.h"

namespace godzilla {

InputParameters
BoundaryCondition::validParams()
{
    InputParameters params = Object::validParams();
    params.add_required_param<std::string>("boundary", "Boundary name");
    return params;
}

BoundaryCondition::BoundaryCondition(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    boundary(get_param<std::string>("boundary"))
{
    _F_;
}

const std::string &
BoundaryCondition::get_boundary_name() const
{
    return this->boundary;
}

} // namespace godzilla
