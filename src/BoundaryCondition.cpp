#include "Godzilla.h"
#include "CallStack.h"
#include "App.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "BoundaryCondition.h"
#include "IndexSet.h"
#include <cassert>

namespace godzilla {

Parameters
BoundaryCondition::parameters()
{
    Parameters params = Object::parameters();
    params.add_required_param<std::string>("boundary", "Boundary name");
    params.add_private_param<const DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

BoundaryCondition::BoundaryCondition(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    dpi(get_param<const DiscreteProblemInterface *>("_dpi")),
    boundary(get_param<std::string>("boundary"))
{
    _F_;
}

const std::string &
BoundaryCondition::get_boundary() const
{
    return this->boundary;
}

const DiscreteProblemInterface *
BoundaryCondition::get_discrete_problem_interface() const
{
    _F_;
    return this->dpi;
}

} // namespace godzilla
