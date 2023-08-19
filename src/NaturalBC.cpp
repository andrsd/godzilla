#include "NaturalBC.h"
#include "CallStack.h"
#include "App.h"
#include "Problem.h"
#include "FEProblemInterface.h"
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

NaturalBC::NaturalBC(const Parameters & params) :
    BoundaryCondition(params),
    fid(-1),
    fepi(dynamic_cast<FEProblemInterface *>(this->dpi))
{
    _F_;
}

void
NaturalBC::create()
{
    _F_;
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
    for (auto & bnd : get_boundary())
        this->dpi->add_boundary_natural(get_name(), bnd, get_field_id(), get_components(), this);
}

void
NaturalBC::add_residual_block(BndResidualFunc * f0, BndResidualFunc * f1)
{
    _F_;
    for (auto & bnd : get_boundary())
        this->fepi->add_boundary_residual_block(this->fid, f0, f1, bnd);
}

void
NaturalBC::add_jacobian_block(Int gid,
                              BndJacobianFunc * g0,
                              BndJacobianFunc * g1,
                              BndJacobianFunc * g2,
                              BndJacobianFunc * g3)
{
    _F_;
    for (auto & bnd : get_boundary())
        this->fepi->add_boundary_jacobian_block(this->fid, gid, g0, g1, g2, g3, bnd);
}

} // namespace godzilla
