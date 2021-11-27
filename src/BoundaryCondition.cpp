#include "Godzilla.h"
#include "BoundaryCondition.h"
#include <assert.h>

namespace godzilla {

PetscErrorCode
__boundary_condition_function(PetscInt dim,
                              PetscReal time,
                              const PetscReal x[],
                              PetscInt nc,
                              PetscScalar u[],
                              void * ctx)
{
    _F_;
    BoundaryCondition * bc = static_cast<BoundaryCondition *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(dim, time, x, nc, u);
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

        ierr = PetscDSAddBoundary(ds,
                                  getBcType(),
                                  getName().c_str(),
                                  label,
                                  n_ids,
                                  ids,
                                  getFieldID(),
                                  getNumComponents(),
                                  getComponents().size() == 0 ? NULL : getComponents().data(),
                                  (void (*)(void)) & __boundary_condition_function,
                                  NULL,
                                  (void *) this,
                                  NULL);
        checkPetscError(ierr);

        ierr = ISRestoreIndices(is, &ids);
        checkPetscError(ierr);
        ierr = ISDestroy(&is);
        checkPetscError(ierr);
    }
}

} // namespace godzilla
