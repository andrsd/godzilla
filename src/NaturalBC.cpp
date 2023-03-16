#include "NaturalBC.h"
#include "CallStack.h"
#include "UnstructuredMesh.h"
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

NaturalBC::NaturalBC(const Parameters & params) : BoundaryCondition(params), wf(nullptr)
{
    _F_;
    auto fepi = dynamic_cast<const FEProblemInterface *>(this->dpi);
    if (fepi)
        this->wf = fepi->get_weak_form();
}

void
NaturalBC::set_up()
{
    _F_;
    const UnstructuredMesh * mesh = this->dpi->get_mesh();
    this->label = mesh->get_face_set_label(this->boundary);
    IndexSet is = IndexSet::values_from_label(this->label);
    is.get_indices();
    this->ids = is.to_std_vector();
    this->dpi->add_boundary_natural(get_name(),
                                    this->label,
                                    this->ids,
                                    this->fid,
                                    get_components(),
                                    this);
    is.restore_indices();
    is.destroy();
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
