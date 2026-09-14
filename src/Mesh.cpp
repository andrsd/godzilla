// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Mesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/StarForest.h"
#include "godzilla/Types.h"

namespace godzilla {

Mesh::Mesh() : PetscObjectWrapper(nullptr) {}

Mesh::Mesh(DM dm) : PetscObjectWrapper(dm) {}

mpi::Communicator
Mesh::get_comm() const
{
    CALL_STACK_MSG();
    MPI_Comm comm;
    PETSC_CHECK(PetscObjectGetComm((PetscObject) this->obj_, &comm));
    return { comm };
}

DM
Mesh::get_dm() const
{
    CALL_STACK_MSG();
    return this->obj_;
}

Dimension
Mesh::get_dimension() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(DMGetDimension(this->obj_, &dim));
    return Dimension::from_int(dim);
}

void
Mesh::set_dimension(Dimension dim)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetDimension(this->obj_, dim));
}

bool
Mesh::has_label(String name) const
{
    CALL_STACK_MSG();
    PetscBool exists = PETSC_FALSE;
    PETSC_CHECK(DMHasLabel(this->obj_, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

Label
Mesh::get_label(String name) const
{
    CALL_STACK_MSG();
    Label label;
    PETSC_CHECK(DMGetLabel(this->obj_, name.c_str(), label));
    label.inc_reference();
    return label;
}

void
Mesh::create_label(String name) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMCreateLabel(this->obj_, name.c_str()));
}

void
Mesh::remove_label(String name)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMRemoveLabel(this->obj_, name.c_str(), nullptr));
}

DM
Mesh::get_coordinate_dm() const
{
    CALL_STACK_MSG();
    DM cdm;
    PETSC_CHECK(DMGetCoordinateDM(this->obj_, &cdm));
    return cdm;
}

Vector
Mesh::get_coordinates() const
{
    CALL_STACK_MSG();
    Vector vec;
    PETSC_CHECK(DMGetCoordinates(this->obj_, vec));
    vec.inc_reference();
    return vec;
}

Vector
Mesh::get_coordinates_local() const
{
    CALL_STACK_MSG();
    Vector vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->obj_, vec));
    vec.inc_reference();
    return vec;
}

Section
Mesh::get_coordinate_section() const
{
    CALL_STACK_MSG();
    Section section;
    PETSC_CHECK(DMGetCoordinateSection(this->obj_, section));
    section.inc_reference();
    return section;
}

Dimension
Mesh::get_coordinate_dim() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(DMGetCoordinateDim(this->obj_, &dim));
    return Dimension::from_int(dim);
}

void
Mesh::set_coordinate_dim(Dimension dim)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetCoordinateDim(this->obj_, dim));
}

void
Mesh::set_coordinates_local(const Vector & c)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetCoordinatesLocal(this->obj_, c));
}

void
Mesh::localize_coordinates() const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLocalizeCoordinates(this->obj_));
}

void
Mesh::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetUp(this->obj_));
}

void
Mesh::set_dm(DM dm)
{
    CALL_STACK_MSG();
    if (this->obj_)
        PETSC_CHECK(DMDestroy(&this->obj_));
    this->obj_ = dm;
}

void
Mesh::set_label_value(const char * name, Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetLabelValue(this->obj_, name, point, value));
}

void
Mesh::clear_label_value(const char * name, Int point, Int value)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMClearLabelValue(this->obj_, name, point, value));
}

void
Mesh::view(PetscViewer viewer)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMView(this->obj_, viewer));
}

Span<const int>
Mesh::get_neighbors() const
{
    Int n;
    const PetscMPIInt * ranks;
    PETSC_CHECK(DMGetNeighbors(this->obj_, &n, &ranks));
    return Span(ranks, n);
}

StarForest
Mesh::locate_points(Vector coords, DMPointLocationType ltype)
{
    StarForest sf;
    PETSC_CHECK(DMLocatePoints(this->obj_, coords, ltype, sf));
    return sf;
}

} // namespace godzilla
