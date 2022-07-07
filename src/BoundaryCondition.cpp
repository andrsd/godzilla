#include "Godzilla.h"
#include "CallStack.h"
#include "App.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "BoundaryCondition.h"
#include <assert.h>

namespace godzilla {

Parameters
BoundaryCondition::valid_params()
{
    Parameters params = Object::valid_params();
    params.add_param<std::string>("field", "", "Field name");
    params.add_required_param<std::string>("boundary", "Boundary name");
    params.add_private_param<const DiscreteProblemInterface *>("_dpi", nullptr);
    return params;
}

BoundaryCondition::BoundaryCondition(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    dpi(get_param<const DiscreteProblemInterface *>("_dpi")),
    dm(nullptr),
    ds(nullptr),
    label(nullptr),
    fid(-1),
    n_ids(0),
    ids(nullptr),
    boundary(get_param<std::string>("boundary"))
{
    _F_;
}

void
BoundaryCondition::create()
{
    _F_;
    Problem * problem = this->app->get_problem();
    assert(problem != nullptr);
    this->dm = problem->get_dm();
    assert(this->dm != nullptr);

    assert(this->dpi != nullptr);
    const UnstructuredMesh * mesh = this->dpi->get_mesh();
    this->label = mesh->get_face_set_label(this->boundary);

    std::vector<std::string> field_names = this->dpi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = this->dpi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        const std::string & field_name = get_param<std::string>("field");
        if (field_name.length() > 0) {
            if (this->dpi->has_field_by_name(field_name))
                this->fid = this->dpi->get_field_id(field_name);
            else
                log_error("Field '%s' does not exists. Typo?", field_name);
        }
        else
            log_error("Use the 'field' parameter to assign this boundary condition to an existing "
                      "field.");
    }
}

const std::string &
BoundaryCondition::get_boundary() const
{
    return this->boundary;
}

PetscInt
BoundaryCondition::get_field_id() const
{
    _F_;
    return this->fid;
}

void
BoundaryCondition::set_up()
{
    _F_;
    PETSC_CHECK(DMGetDS(this->dm, &this->ds));
    IS is;
    PETSC_CHECK(DMLabelGetValueIS(this->label, &is));
    PETSC_CHECK(ISGetSize(is, &this->n_ids));
    PETSC_CHECK(ISGetIndices(is, &this->ids));
    set_up_callback();
    PETSC_CHECK(ISRestoreIndices(is, &this->ids));
    this->ids = nullptr;
    PETSC_CHECK(ISDestroy(&is));
}

} // namespace godzilla
