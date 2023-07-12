#include "NaturalRiemannBC.h"
#include "CallStack.h"
#include "App.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "UnstructuredMesh.h"
#include <cassert>

namespace godzilla {

static PetscErrorCode
natural_riemann_boundary_condition_function(Real time,
                                            const Real * c,
                                            const Real * n,
                                            const Scalar * xI,
                                            Scalar * xG,
                                            void * ctx)
{
    _F_;
    auto * bc = static_cast<NaturalRiemannBC *>(ctx);
    assert(bc != nullptr);
    bc->evaluate(time, c, n, xI, xG);
    return 0;
}

Parameters
NaturalRiemannBC::parameters()
{
    Parameters params = BoundaryCondition::parameters();
    params.add_param<std::string>("field", "", "Field name");
    return params;
}

NaturalRiemannBC::NaturalRiemannBC(const Parameters & params) : BoundaryCondition(params)
{
    _F_;
}

void
NaturalRiemannBC::create()
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

    auto mesh = get_mesh();
    this->label = mesh->get_face_set_label(this->boundary);
    if (!this->label.is_null()) {
        auto is = this->label.get_values();
        is.get_indices();
        this->ids = is.to_std_vector();
        is.restore_indices();
        is.destroy();
    }
}

const Label &
NaturalRiemannBC::get_label() const
{
    _F_;
    return this->label;
}

const std::vector<Int> &
NaturalRiemannBC::get_ids() const
{
    _F_;
    return this->ids;
}

Int
NaturalRiemannBC::get_field_id() const
{
    _F_;
    return this->fid;
}

void
NaturalRiemannBC::set_up()
{
    _F_;
    auto mesh = get_problem()->get_mesh();
    this->dpi->add_boundary_natural_riemann(get_name(),
                                            this->label,
                                            this->ids,
                                            this->fid,
                                            get_components(),
                                            natural_riemann_boundary_condition_function,
                                            nullptr,
                                            this);
}

} // namespace godzilla
