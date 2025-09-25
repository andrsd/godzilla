// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/StructuredMesh.h"
#include "godzilla/CallStack.h"
#include <petscdmda.h>

namespace godzilla {

StructuredMesh::StructuredMesh(const mpi::Communicator & comm) : Mesh(nullptr)
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMDACreate(comm, &dm));
    set_dm(dm);
}

StructuredMesh::StructuredMesh(DM dm) : Mesh(dm)
{
    CALL_STACK_MSG();
}

StructuredMesh
StructuredMesh::clone() const
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMClone(get_dm(), &dm));
    return StructuredMesh(dm);
}

StructuredMesh
StructuredMesh::create_1d(const mpi::Communicator comm, DMBoundaryType bx, Int M, Int dof, Int s)
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMDACreate1d(comm, bx, M, dof, s, NULL, &dm));
    return StructuredMesh(dm);
}

} // namespace godzilla
