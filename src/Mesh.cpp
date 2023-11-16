#include "godzilla/Godzilla.h"
#include "godzilla/Mesh.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
Mesh::parameters()
{
    Parameters params = Object::parameters();
    return params;
}

Mesh::Mesh(const Parameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    _dm(nullptr),
    dim(-1)
{
}

Mesh::~Mesh()
{
    _F_;
    if (this->_dm)
        PETSC_CHECK(DMDestroy(&this->_dm));
}

DM
Mesh::get_dm() const
{
    _F_;
    return dm();
}

DM
Mesh::dm() const
{
    _F_;
    return this->_dm;
}

Int
Mesh::get_dimension() const
{
    _F_;
    return this->dim;
}

bool
Mesh::has_label(const std::string & name) const
{
    _F_;
    PetscBool exists = PETSC_FALSE;
    PETSC_CHECK(DMHasLabel(this->_dm, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

Label
Mesh::get_label(const std::string & name) const
{
    _F_;
    DMLabel label;
    PETSC_CHECK(DMGetLabel(this->_dm, name.c_str(), &label));
    return Label(label);
}

Label
Mesh::create_label(const std::string & name) const
{
    _F_;
    DMLabel label;
    PETSC_CHECK(DMCreateLabel(this->_dm, name.c_str()));
    PETSC_CHECK(DMGetLabel(this->_dm, name.c_str(), &label));
    return Label(label);
}

void
Mesh::remove_label(const std::string & name)
{
    _F_;
    PETSC_CHECK(DMRemoveLabel(this->_dm, name.c_str(), nullptr));
}

DM
Mesh::get_coordinate_dm() const
{
    _F_;
    DM cdm;
    PETSC_CHECK(DMGetCoordinateDM(this->_dm, &cdm));
    return cdm;
}

Vector
Mesh::get_coordinates() const
{
    _F_;
    Vec vec;
    PETSC_CHECK(DMGetCoordinates(this->_dm, &vec));
    return { vec };
}

Vector
Mesh::get_coordinates_local() const
{
    _F_;
    Vec vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->_dm, &vec));
    return { vec };
}

Section
Mesh::get_coordinate_section() const
{
    _F_;
    PetscSection section;
    PETSC_CHECK(DMGetCoordinateSection(this->_dm, &section));
    return { section };
}

void
Mesh::set_up()
{
    _F_;
    PETSC_CHECK(DMSetUp(this->_dm));
}

} // namespace godzilla
