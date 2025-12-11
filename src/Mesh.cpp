// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Mesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Types.h"

namespace godzilla {

Mesh::Mesh() : PetscObjectWrapper(nullptr) {}

Mesh::Mesh(DM dm) : PetscObjectWrapper(dm) {}

mpi::Communicator
Mesh::get_comm() const
{
    CALL_STACK_MSG();
    MPI_Comm comm;
    PETSC_CHECK(PetscObjectGetComm((PetscObject) this->obj, &comm));
    return { comm };
}

DM
Mesh::get_dm() const
{
    CALL_STACK_MSG();
    return this->obj;
}

Dimension
Mesh::get_dimension() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(DMGetDimension(this->obj, &dim));
    return Dimension::from_int(dim);
}

void
Mesh::set_dimension(Dimension dim)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetDimension(this->obj, dim));
}

bool
Mesh::has_label(const String & name) const
{
    CALL_STACK_MSG();
    PetscBool exists = PETSC_FALSE;
    PETSC_CHECK(DMHasLabel(this->obj, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

Label
Mesh::get_label(const String & name) const
{
    CALL_STACK_MSG();
    Label label;
    PETSC_CHECK(DMGetLabel(this->obj, name.c_str(), label));
    label.inc_reference();
    return label;
}

void
Mesh::create_label(const String & name) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMCreateLabel(this->obj, name.c_str()));
}

void
Mesh::remove_label(const String & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMRemoveLabel(this->obj, name.c_str(), nullptr));
}

DM
Mesh::get_coordinate_dm() const
{
    CALL_STACK_MSG();
    DM cdm;
    PETSC_CHECK(DMGetCoordinateDM(this->obj, &cdm));
    return cdm;
}

Vector
Mesh::get_coordinates() const
{
    CALL_STACK_MSG();
    Vector vec;
    PETSC_CHECK(DMGetCoordinates(this->obj, vec));
    vec.inc_reference();
    return vec;
}

Vector
Mesh::get_coordinates_local() const
{
    CALL_STACK_MSG();
    Vector vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->obj, vec));
    vec.inc_reference();
    return vec;
}

Section
Mesh::get_coordinate_section() const
{
    CALL_STACK_MSG();
    Section section;
    PETSC_CHECK(DMGetCoordinateSection(this->obj, section));
    section.inc_reference();
    return section;
}

Dimension
Mesh::get_coordinate_dim() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(DMGetCoordinateDim(this->obj, &dim));
    return Dimension::from_int(dim);
}

void
Mesh::set_coordinate_dim(Dimension dim)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetCoordinateDim(this->obj, dim));
}

void
Mesh::set_coordinates_local(const Vector & c)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetCoordinatesLocal(this->obj, c));
}

void
Mesh::localize_coordinates() const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLocalizeCoordinates(this->obj));
}

void
Mesh::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetUp(this->obj));
}

void
Mesh::set_dm(DM dm)
{
    CALL_STACK_MSG();
    if (this->obj)
        PETSC_CHECK(DMDestroy(&this->obj));
    this->obj = dm;
}

void
Mesh::set_label_value(const char * name, Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetLabelValue(this->obj, name, point, value));
}

void
Mesh::clear_label_value(const char * name, Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMClearLabelValue(this->obj, name, point, value));
}

void
Mesh::view(PetscViewer viewer)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMView(this->obj, viewer));
}

std::vector<int>
Mesh::get_neighbors() const
{
    Int n;
    const PetscMPIInt * ranks;
    PETSC_CHECK(DMGetNeighbors(this->obj, &n, &ranks));
    std::vector<int> res(n);
    for (Int i = 0; i < n; ++i)
        res[i] = ranks[i];
    return res;
}

} // namespace godzilla
