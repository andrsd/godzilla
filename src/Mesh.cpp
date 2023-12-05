// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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

Mesh::Mesh(const Parameters & parameters) : Object(parameters), PrintInterface(this), dm(nullptr) {}

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
    Int dim;
    PETSC_CHECK(DMGetDimension(this->dm, &dim));
    return dim;
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
    _F_;
    DMLabel label;
    PETSC_CHECK(DMCreateLabel(this->dm, name.c_str()));
    PETSC_CHECK(DMGetLabel(this->dm, name.c_str(), &label));
    return Label(label);
}

void
Mesh::remove_label(const std::string & name)
{
    _F_;
    PETSC_CHECK(DMRemoveLabel(this->dm, name.c_str(), nullptr));
}

DM
Mesh::get_coordinate_dm() const
{
    _F_;
    DM cdm;
    PETSC_CHECK(DMGetCoordinateDM(this->dm, &cdm));
    return cdm;
}

Vector
Mesh::get_coordinates() const
{
    _F_;
    Vec vec;
    PETSC_CHECK(DMGetCoordinates(this->dm, &vec));
    return { vec };
}

Vector
Mesh::get_coordinates_local() const
{
    _F_;
    Vec vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->dm, &vec));
    return { vec };
}

Section
Mesh::get_coordinate_section() const
{
    _F_;
    PetscSection section;
    PETSC_CHECK(DMGetCoordinateSection(this->dm, &section));
    return { section };
}

void
Mesh::set_up()
{
    _F_;
    PETSC_CHECK(DMSetUp(this->dm));
}

void
Mesh::set_dm(DM dm)
{
    _F_;
    if (this->dm)
        PETSC_CHECK(DMDestroy(&this->dm));
    this->dm = dm;
}

void
Mesh::set_label_value(const char * name, Int point, Int value)
{
    _F_;
    PETSC_CHECK(DMSetLabelValue(this->dm, name, point, value));
}

void
Mesh::clear_label_value(const char * name, Int point, Int value)
{
    _F_;
    PETSC_CHECK(DMClearLabelValue(this->dm, name, point, value));
}

} // namespace godzilla
