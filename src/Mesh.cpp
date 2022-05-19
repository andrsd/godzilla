#include "Godzilla.h"
#include "Mesh.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
Mesh::valid_params()
{
    InputParameters params = Object::valid_params();
    return params;
}

Mesh::Mesh(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    dm(NULL),
    dim(-1)
{
}

Mesh::~Mesh()
{
    _F_;
    if (this->dm) {
        PetscErrorCode ierr;
        ierr = DMDestroy(&this->dm);
        check_petsc_error(ierr);
    }
}

DM
Mesh::get_dm() const
{
    _F_;
    return this->dm;
}

PetscInt
Mesh::get_dimension() const
{
    _F_;
    return this->dim;
}

void
Mesh::create()
{
    _F_;
    PetscErrorCode ierr;

    create_dm();
    ierr = DMSetUp(this->dm);
    check_petsc_error(ierr);
    ierr = DMGetDimension(this->dm, &this->dim);
    check_petsc_error(ierr);
    distribute();
}

bool
Mesh::has_label(const std::string & name) const
{
    _F_;
    PetscErrorCode ierr;
    PetscBool exists = PETSC_FALSE;
    ierr = DMHasLabel(this->dm, name.c_str(), &exists);
    check_petsc_error(ierr);
    return exists == PETSC_TRUE;
}

} // namespace godzilla
