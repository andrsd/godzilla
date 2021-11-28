#include "Godzilla.h"
#include "BoundaryCondition.h"

namespace godzilla {

InputParameters
BoundaryCondition::validParams()
{
    InputParameters params = Object::validParams();
    params.addRequiredParam<std::string>("boundary", "Boundary name");
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
    boundary(getParam<std::string>("boundary"))
{
    _F_;
}

const std::string &
BoundaryCondition::getBoundary() const
{
    return this->boundary;
}

void
BoundaryCondition::setUp(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    this->dm = dm;

    ierr = DMGetDS(dm, &this->ds);
    checkPetscError(ierr);

    IS is;
    ierr = DMGetLabelIdIS(dm, this->boundary.c_str(), &is);
    checkPetscError(ierr);

    ierr = DMGetLabel(dm, this->boundary.c_str(), &this->label);
    checkPetscError(ierr);

    ierr = ISGetSize(is, &this->n_ids);
    checkPetscError(ierr);

    ierr = ISGetIndices(is, &this->ids);
    checkPetscError(ierr);

    setUpCallback();

    ierr = ISRestoreIndices(is, &this->ids);
    checkPetscError(ierr);
    this->ids = nullptr;

    ierr = ISDestroy(&is);
    checkPetscError(ierr);
}

} // namespace godzilla
