#include "Godzilla.h"
#include "CallStack.h"
#include "App.h"
#include "Mesh.h"
#include "Problem.h"
#include "FEProblemInterface.h"
#include "BoundaryCondition.h"
#include <assert.h>

namespace godzilla {

InputParameters
BoundaryCondition::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_param<std::string>("field", "", "Field name");
    params.add_required_param<std::string>("boundary", "Boundary name");
    params.add_private_param<const FEProblemInterface *>("_fepi", nullptr);
    return params;
}

BoundaryCondition::BoundaryCondition(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    fepi(get_param<const FEProblemInterface *>("_fepi")),
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

    const Mesh * mesh = problem->get_mesh();
    this->label = mesh->get_label(this->boundary);

    assert(this->fepi != nullptr);
    std::vector<std::string> field_names = this->fepi->get_field_names();
    if (field_names.size() == 1) {
        this->fid = this->fepi->get_field_id(field_names[0]);
    }
    else if (field_names.size() > 1) {
        const std::string & field_name = get_param<std::string>("field");
        if (field_name.length() > 0) {
            if (this->fepi->has_field_by_name(field_name))
                this->fid = this->fepi->get_field_id(field_name);
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
    PetscErrorCode ierr;

    ierr = DMGetDS(this->dm, &this->ds);
    check_petsc_error(ierr);

    IS is;
    ierr = DMGetLabelIdIS(this->dm, this->boundary.c_str(), &is);
    check_petsc_error(ierr);

    ierr = ISGetSize(is, &this->n_ids);
    check_petsc_error(ierr);

    ierr = ISGetIndices(is, &this->ids);
    check_petsc_error(ierr);

    set_up_callback();

    ierr = ISRestoreIndices(is, &this->ids);
    check_petsc_error(ierr);
    this->ids = nullptr;

    ierr = ISDestroy(&is);
    check_petsc_error(ierr);
}

} // namespace godzilla
