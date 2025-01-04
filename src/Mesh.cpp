// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/Mesh.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Mesh::Mesh() : dm(nullptr) {}

Mesh::Mesh(DM dm) : dm(dm) {}

Mesh::~Mesh()
{
    CALL_STACK_MSG();
    if (this->dm)
        PETSC_CHECK(DMDestroy(&this->dm));
}

mpi::Communicator
Mesh::get_comm() const
{
    CALL_STACK_MSG();
    MPI_Comm comm;
    PETSC_CHECK(PetscObjectGetComm((PetscObject) this->dm, &comm));
    return { comm };
}

DM
Mesh::get_dm() const
{
    CALL_STACK_MSG();
    return this->dm;
}

Int
Mesh::get_dimension() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(DMGetDimension(this->dm, &dim));
    return dim;
}

void
Mesh::set_dimension(Int dim)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetDimension(this->dm, dim));
}

bool
Mesh::has_label(const std::string & name) const
{
    CALL_STACK_MSG();
    PetscBool exists = PETSC_FALSE;
    PETSC_CHECK(DMHasLabel(this->dm, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

Label
Mesh::get_label(const std::string & name) const
{
    CALL_STACK_MSG();
    DMLabel label;
    PETSC_CHECK(DMGetLabel(this->dm, name.c_str(), &label));
    return Label(label);
}

void
Mesh::create_label(const std::string & name) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMCreateLabel(this->dm, name.c_str()));
}

void
Mesh::remove_label(const std::string & name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMRemoveLabel(this->dm, name.c_str(), nullptr));
}

DM
Mesh::get_coordinate_dm() const
{
    CALL_STACK_MSG();
    DM cdm;
    PETSC_CHECK(DMGetCoordinateDM(this->dm, &cdm));
    return cdm;
}

Vector
Mesh::get_coordinates() const
{
    CALL_STACK_MSG();
    Vec vec;
    PETSC_CHECK(DMGetCoordinates(this->dm, &vec));
    return { vec };
}

Vector
Mesh::get_coordinates_local() const
{
    CALL_STACK_MSG();
    Vec vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->dm, &vec));
    return { vec };
}

Section
Mesh::get_coordinate_section() const
{
    CALL_STACK_MSG();
    PetscSection section;
    PETSC_CHECK(DMGetCoordinateSection(this->dm, &section));
    return { section };
}

void
Mesh::set_coordinate_dim(Int dim)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetCoordinateDim(this->dm, dim));
}

void
Mesh::set_coordinates_local(const Vector & c)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetCoordinatesLocal(this->dm, c));
}

void
Mesh::localize_coordinates() const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLocalizeCoordinates(this->dm));
}

void
Mesh::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetUp(this->dm));
}

void
Mesh::set_dm(DM dm)
{
    CALL_STACK_MSG();
    if (this->dm)
        PETSC_CHECK(DMDestroy(&this->dm));
    this->dm = dm;
}

void
Mesh::set_label_value(const char * name, Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetLabelValue(this->dm, name, point, value));
}

void
Mesh::clear_label_value(const char * name, Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMClearLabelValue(this->dm, name, point, value));
}

void
Mesh::view(PetscViewer viewer)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMView(this->dm, viewer));
}

std::vector<int>
Mesh::get_neighbors() const
{
    Int n;
    const PetscMPIInt * ranks;
    PETSC_CHECK(DMGetNeighbors(this->dm, &n, &ranks));
    std::vector<int> res(n);
    for (Int i = 0; i < n; i++)
        res[i] = ranks[i];
    return res;
}

} // namespace godzilla
