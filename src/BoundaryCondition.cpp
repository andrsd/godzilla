#include "Godzilla.h"
#include "CallStack.h"
#include "App.h"
#include "Mesh.h"
#include "Problem.h"
#include "BoundaryCondition.h"
#include <assert.h>

namespace godzilla {

InputParameters
BoundaryCondition::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_required_param<std::string>("boundary", "Boundary name");
    return params;
}

BoundaryCondition::BoundaryCondition(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    dm(nullptr),
    ds(nullptr),
    label(nullptr),
    n_ids(0),
    ids(nullptr),
    boundary(get_param<std::string>("boundary"))
{
    _F_;
}

void
BoundaryCondition::create()
{
    _F_;
    Problem * problem = this->app->get_problem();
    assert(problem != nullptr);
    this->dm = problem->get_dm();
    assert(this->dm != nullptr);

    const Mesh * mesh = problem->get_mesh();
    this->label = mesh->get_label(this->boundary);
}

const std::string &
BoundaryCondition::get_boundary() const
{
    return this->boundary;
}

void
BoundaryCondition::set_up()
{
    _F_;
    PetscErrorCode ierr;

    ierr = DMGetDS(this->dm, &this->ds);
    check_petsc_error(ierr);

    IS is;
    ierr = DMGetLabelIdIS(this->dm, this->boundary.c_str(), &is);
    check_petsc_error(ierr);

    ierr = ISGetSize(is, &this->n_ids);
    check_petsc_error(ierr);

    ierr = ISGetIndices(is, &this->ids);
    check_petsc_error(ierr);

    set_up_callback();

    ierr = ISRestoreIndices(is, &this->ids);
    check_petsc_error(ierr);
    this->ids = nullptr;

    ierr = ISDestroy(&is);
    check_petsc_error(ierr);
}

} // namespace godzilla
