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
    if (this->dm)
        PETSC_CHECK(DMDestroy(&this->dm));
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
    create_dm();
    PETSC_CHECK(DMSetFromOptions(this->dm));
    PETSC_CHECK(DMViewFromOptions(dm, NULL, "-dm_view"));
    PETSC_CHECK(DMSetUp(this->dm));
    PETSC_CHECK(DMGetDimension(this->dm, &this->dim));
    distribute();
}

bool
Mesh::has_label(const std::string & name) const
{
    _F_;
    PetscBool exists = PETSC_FALSE;
    PETSC_CHECK(DMHasLabel(this->dm, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

DMLabel
Mesh::get_label(const std::string & name) const
{
    _F_;
    DMLabel label;
    PETSC_CHECK(DMGetLabel(this->dm, name.c_str(), &label));
    return label;
}

} // namespace godzilla
