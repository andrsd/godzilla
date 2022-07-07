#include "NaturalBC.h"
#include "CallStack.h"

namespace godzilla {

Parameters
NaturalBC::parameters()
{
    Parameters params = BoundaryCondition::parameters();
    return params;
}

NaturalBC::NaturalBC(const Parameters & params) : BoundaryCondition(params), wf(nullptr), bd(-1)
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
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   get_bc_type(),
                                   get_name().c_str(),
                                   this->label,
                                   this->n_ids,
                                   this->ids,
                                   this->fid,
                                   get_num_components(),
                                   get_num_components() == 0 ? nullptr : get_components().data(),
                                   nullptr,
                                   nullptr,
                                   (void *) this,
                                   &this->bd));

    PETSC_CHECK(PetscDSGetBoundary(ds,
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
                                   nullptr));

    set_up_weak_form();
}

void
NaturalBC::set_residual_block(PetscFEBndResidualFunc * f0, PetscFEBndResidualFunc * f1)
{
    _F_;
    for (PetscInt i = 0; i < this->n_ids; i++) {
        PetscInt id = this->ids[i];
        PETSC_CHECK(
            PetscWeakFormSetIndexBdResidual(this->wf, this->label, id, this->fid, 0, 0, f0, 0, f1));
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
        PETSC_CHECK(PetscWeakFormSetIndexBdJacobian(this->wf,
                                                    this->label,
                                                    id,
                                                    this->fid,
                                                    gid,
                                                    0,
                                                    0,
                                                    g0,
                                                    0,
                                                    g1,
                                                    0,
                                                    g2,
                                                    0,
                                                    g3));
    }
}

} // namespace godzilla
