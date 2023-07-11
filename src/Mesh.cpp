#include "Godzilla.h"
#include "Mesh.h"
#include "CallStack.h"
#include "Section.h"

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
    dm(nullptr),
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
    PETSC_CHECK(DMHasLabel(this->dm, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

Label
Mesh::get_label(const std::string & name) const
{
    _F_;
    DMLabel label;
    PETSC_CHECK(DMGetLabel(this->dm, name.c_str(), &label));
    return Label(label);
}

Label
Mesh::create_label(const std::string & name) const
{
    DMLabel label;
    PETSC_CHECK(DMCreateLabel(this->dm, name.c_str()));
    PETSC_CHECK(DMGetLabel(this->dm, name.c_str(), &label));
    return Label(label);
}

DM
Mesh::get_coordinate_dm() const
{
    DM cdm;
    PETSC_CHECK(DMGetCoordinateDM(this->dm, &cdm));
    return cdm;
}

Vector
Mesh::get_coordinates() const
{
    Vec vec;
    PETSC_CHECK(DMGetCoordinates(this->dm, &vec));
    return { vec };
}

Vector
Mesh::get_coordinates_local() const
{
    Vec vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->dm, &vec));
    return { vec };
}

Section
Mesh::get_local_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetLocalSection(this->dm, &section));
    return { section };
}

void
Mesh::set_local_section(const Section & section) const
{
    _F_;
    PETSC_CHECK(DMSetLocalSection(this->dm, section));
}

Section
Mesh::get_global_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetGlobalSection(this->dm, &section));
    return { section };
}

void
Mesh::set_global_section(const Section & section) const
{
    _F_;
    PETSC_CHECK(DMSetGlobalSection(this->dm, section));
}

} // namespace godzilla
