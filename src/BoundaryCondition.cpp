#include "Godzilla.h"
#include "CallStack.h"
#include "App.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "BoundaryCondition.h"

namespace godzilla {

Parameters
BoundaryCondition::parameters()
{
    Parameters params = Object::parameters();
    params.add_required_param<std::string>("boundary", "Boundary name");
    params.add_private_param<DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

BoundaryCondition::BoundaryCondition(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    mesh(nullptr),
    dpi(get_param<DiscreteProblemInterface *>("_dpi")),
    boundary(get_param<std::string>("boundary"))
{
    _F_;
    this->mesh = dynamic_cast<const UnstructuredMesh *>(get_problem()->get_mesh());
}

const UnstructuredMesh *
BoundaryCondition::get_mesh() const
{
    _F_;
    return this->mesh;
}

const Problem *
BoundaryCondition::get_problem() const
{
    return this->dpi->get_problem();
}

const std::string &
BoundaryCondition::get_boundary() const
{
    return this->boundary;
}

DiscreteProblemInterface *
BoundaryCondition::get_discrete_problem_interface() const
{
    _F_;
    return this->dpi;
}

} // namespace godzilla
