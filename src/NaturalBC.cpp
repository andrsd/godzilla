#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
NaturalBC::validParams()
{
    InputParameters params = BoundaryCondition::validParams();
    return params;
}

NaturalBC::NaturalBC(const InputParameters & params) :
    BoundaryCondition(params),
    wf(nullptr),
    bd(-1)
{
    _F_;
}

DMBoundaryConditionType
NaturalBC::getBcType() const
{
    _F_;
    return DM_BC_NATURAL;
}

void
NaturalBC::setUpCallback()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSAddBoundary(this->ds,
                              getBcType(),
                              getName().c_str(),
                              this->label,
                              this->n_ids,
                              this->ids,
                              getFieldID(),
                              getNumComponents(),
                              getNumComponents() == 0 ? nullptr : getComponents().data(),
                              nullptr,
                              nullptr,
                              (void *) this,
                              &this->bd);
    checkPetscError(ierr);

    ierr = PetscDSGetBoundary(ds,
                              this->bd,
                              &this->wf,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr);
    checkPetscError(ierr);

    onSetWeakForm();
}

void
NaturalBC::setResidualBlock(PetscFEBndResidualFunc * f0, PetscFEBndResidualFunc * f1)
{
    _F_;
    for (PetscInt i = 0; i < this->n_ids; i++) {
        PetscInt id = this->ids[i];
        PetscWeakFormSetIndexBdResidual(this->wf, this->label, id, getFieldID(), 0, 0, f0, 0, f1);
    }
}

void
NaturalBC::setJacobianBlock(PetscInt gid,
                            PetscFEBndJacobianFunc * g0,
                            PetscFEBndJacobianFunc * g1,
                            PetscFEBndJacobianFunc * g2,
                            PetscFEBndJacobianFunc * g3)
{
    _F_;
    for (PetscInt i = 0; i < this->n_ids; i++) {
        PetscInt id = this->ids[i];
        PetscWeakFormSetIndexBdJacobian(this->wf,
                                        this->label,
                                        id,
                                        getFieldID(),
                                        gid,
                                        0,
                                        0,
                                        g0,
                                        0,
                                        g1,
                                        0,
                                        g2,
                                        0,
                                        g3);
    }
}

} // namespace godzilla
