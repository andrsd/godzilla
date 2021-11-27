#include "Godzilla.h"
#include "BoundaryCondition.h"

namespace godzilla {

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

void
BoundaryCondition::setUp(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    PetscDS ds;
    ierr = DMGetDS(dm, &ds);
    checkPetscError(ierr);

    for (auto & bnd_name : getBoundary()) {
        DMLabel label;
        ierr = DMGetLabel(dm, bnd_name.c_str(), &label);
        checkPetscError(ierr);

        IS is;
        ierr = DMGetLabelIdIS(dm, bnd_name.c_str(), &is);
        checkPetscError(ierr);

        PetscInt n_ids;
        ierr = ISGetSize(is, &n_ids);
        checkPetscError(ierr);

        const PetscInt * ids = nullptr;
        ierr = ISGetIndices(is, &ids);
        checkPetscError(ierr);

        setUpCallback(ds, label, n_ids, ids);

        ierr = ISRestoreIndices(is, &ids);
        checkPetscError(ierr);
        ierr = ISDestroy(&is);
        checkPetscError(ierr);
    }
}

} // namespace godzilla
