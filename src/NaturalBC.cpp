#include "NaturalBC.h"
#include "CallStack.h"
#include "DiscreteProblemInterface.h"
#include "WeakForm.h"

namespace godzilla {

Parameters
NaturalBC::parameters()
{
    Parameters params = BoundaryCondition::parameters();
    return params;
}

NaturalBC::NaturalBC(const Parameters & params) :
    BoundaryCondition(params),
    wf(this->dpi->get_weak_form()),
    bd(-1)
{
    _F_;
}

void
NaturalBC::add_boundary()
{
    _F_;
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_NATURAL,
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
}

void
NaturalBC::set_residual_block(BndResidualFunc * f0, BndResidualFunc * f1)
{
    _F_;
    for (PetscInt i = 0; i < this->n_ids; i++) {
        PetscInt id = this->ids[i];
        this->wf->add(PETSC_WF_BDF0, this->label, id, this->fid, 0, f0);
        this->wf->add(PETSC_WF_BDF1, this->label, id, this->fid, 0, f1);
    }
}

void
NaturalBC::set_jacobian_block(PetscInt gid,
                              BndJacobianFunc * g0,
                              BndJacobianFunc * g1,
                              BndJacobianFunc * g2,
                              BndJacobianFunc * g3)
{
    _F_;
    for (PetscInt i = 0; i < this->n_ids; i++) {
        PetscInt id = this->ids[i];
        this->wf->add(PETSC_WF_BDG0, this->label, id, this->fid, gid, 0, g0);
        this->wf->add(PETSC_WF_BDG1, this->label, id, this->fid, gid, 0, g1);
        this->wf->add(PETSC_WF_BDG2, this->label, id, this->fid, gid, 0, g2);
        this->wf->add(PETSC_WF_BDG3, this->label, id, this->fid, gid, 0, g3);
    }
}

} // namespace godzilla
