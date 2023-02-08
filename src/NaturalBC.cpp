#include "NaturalBC.h"
#include "CallStack.h"
#include "DiscreteProblemInterface.h"
#include "WeakForm.h"
#include "BndResidualFunc.h"
#include "BndJacobianFunc.h"

namespace godzilla {

Parameters
NaturalBC::parameters()
{
    Parameters params = BoundaryCondition::parameters();
    return params;
}

NaturalBC::NaturalBC(const Parameters & params) :
    BoundaryCondition(params),
    wf(this->dpi->get_weak_form())
{
    _F_;
}

void
NaturalBC::add_boundary()
{
    _F_;
    this->dpi->add_boundary_natural(get_name(),
                                    this->label,
                                    this->ids,
                                    this->fid,
                                    get_components(),
                                    this);
}

void
NaturalBC::set_residual_block(BndResidualFunc * f0, BndResidualFunc * f1)
{
    _F_;
    for (auto & id : this->ids) {
        this->wf->add(PETSC_WF_BDF0, this->label, id, this->fid, 0, f0);
        this->wf->add(PETSC_WF_BDF1, this->label, id, this->fid, 0, f1);
    }
}

void
NaturalBC::set_jacobian_block(Int gid,
                              BndJacobianFunc * g0,
                              BndJacobianFunc * g1,
                              BndJacobianFunc * g2,
                              BndJacobianFunc * g3)
{
    _F_;
    for (auto & id : this->ids) {
        this->wf->add(PETSC_WF_BDG0, this->label, id, this->fid, gid, 0, g0);
        this->wf->add(PETSC_WF_BDG1, this->label, id, this->fid, gid, 0, g1);
        this->wf->add(PETSC_WF_BDG2, this->label, id, this->fid, gid, 0, g2);
        this->wf->add(PETSC_WF_BDG3, this->label, id, this->fid, gid, 0, g3);
    }
}

} // namespace godzilla
