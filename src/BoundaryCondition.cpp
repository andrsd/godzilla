#include "Godzilla.h"
#include "CallStack.h"
#include "App.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "BoundaryCondition.h"

namespace godzilla {

Parameters
BoundaryCondition::parameters()
{
    Parameters params = Object::parameters();
    params.add_required_param<std::vector<std::string>>("boundary", "Boundary name");
    params.add_private_param<DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

BoundaryCondition::BoundaryCondition(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    dpi(get_param<DiscreteProblemInterface *>("_dpi")),
    boundary(get_param<std::vector<std::string>>("boundary"))
{
    _F_;
}

Problem *
BoundaryCondition::get_problem() const
{
    _F_;
    return this->dpi->get_problem();
}

const std::vector<std::string> &
BoundaryCondition::get_boundary() const
{
    _F_;
    return this->boundary;
}

DiscreteProblemInterface *
BoundaryCondition::get_discrete_problem_interface() const
{
    _F_;
    return this->dpi;
}

} // namespace godzilla
