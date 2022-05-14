#include "Godzilla.h"
#include "BoundaryCondition.h"

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

const std::string &
BoundaryCondition::get_boundary() const
{
    return this->boundary;
}

void
BoundaryCondition::set_up(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    this->dm = dm;

    ierr = DMGetDS(dm, &this->ds);
    check_petsc_error(ierr);

    IS is;
    ierr = DMGetLabelIdIS(dm, this->boundary.c_str(), &is);
    check_petsc_error(ierr);

    ierr = DMGetLabel(dm, this->boundary.c_str(), &this->label);
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
