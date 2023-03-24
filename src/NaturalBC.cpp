#include "NaturalBC.h"
#include "CallStack.h"
#include "App.h"
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
    params.add_param<std::string>("field", "", "Field name");
    return params;
}

NaturalBC::NaturalBC(const Parameters & params) : BoundaryCondition(params), fid(-1), wf(nullptr)
{
    _F_;
    auto fepi = dynamic_cast<const FEProblemInterface *>(this->dpi);
    if (fepi)
        this->wf = fepi->get_weak_form();
}

void
NaturalBC::create()
{
    _F_;
    assert(this->app->get_problem() != nullptr);
    assert(this->dpi != nullptr);

    std::vector<std::string> field_names = this->dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = this->dpi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        const auto & field_name = get_param<std::string>("field");
        if (field_name.length() > 0) {
            if (this->dpi->has_field_by_name(field_name))
                this->fid = this->dpi->get_field_id(field_name);
            else
                log_error("Field '{}' does not exists. Typo?", field_name);
        }
        else
            log_error("Use the 'field' parameter to assign this boundary condition to an existing "
                      "field.");
    }
}

Int
NaturalBC::get_field_id() const
{
    _F_;
    return this->fid;
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
