#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
NaturalBC::valid_params()
{
    InputParameters params = BoundaryCondition::valid_params();
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
NaturalBC::get_bc_type() const
{
    _F_;
    return DM_BC_NATURAL;
}

void
NaturalBC::set_up_callback()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSAddBoundary(this->ds,
                              get_bc_type(),
                              get_name().c_str(),
                              this->label,
                              this->n_ids,
                              this->ids,
                              get_field_id(),
                              get_num_components(),
                              get_num_components() == 0 ? nullptr : get_components().data(),
                              nullptr,
                              nullptr,
                              (void *) this,
                              &this->bd);
    check_petsc_error(ierr);

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
    check_petsc_error(ierr);

    set_up_weak_form();
}

void
NaturalBC::set_residual_block(PetscFEBndResidualFunc * f0, PetscFEBndResidualFunc * f1)
{
    _F_;
    for (PetscInt i = 0; i < this->n_ids; i++) {
        PetscInt id = this->ids[i];
        PetscWeakFormSetIndexBdResidual(this->wf, this->label, id, get_field_id(), 0, 0, f0, 0, f1);
    }
}

void
NaturalBC::set_jacobian_block(PetscInt gid,
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
                                        get_field_id(),
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
