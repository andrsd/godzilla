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

void
StructuredMesh::set_boundary_type(DMBoundaryType bx, DMBoundaryType by, DMBoundaryType bz)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMDASetBoundaryType(get_dm(), bx, by, bz));
}

void
StructuredMesh::set_stencil_type(DMDAStencilType stype)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMDASetStencilType(get_dm(), stype));
}

void
StructuredMesh::set_overlap(Int x, Int y, Int z)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMDASetOverlap(get_dm(), x, y, z));
}

void
StructuredMesh::set_sizes(Int M, Int N, Int P)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMDASetSizes(get_dm(), M, N, P));
}

StructuredMesh
StructuredMesh::create_1d(const mpi::Communicator comm, DMBoundaryType bx, Int M, Int dof, Int s)
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMDACreate1d(comm, bx, M, dof, s, NULL, &dm));
    return StructuredMesh(dm);
}

StructuredMesh
StructuredMesh::create_2d(const mpi::Communicator comm,
                          DMBoundaryType bx,
                          DMBoundaryType by,
                          DMDAStencilType stencil_type,
                          Int M,
                          Int N,
                          Int m,
                          Int n,
                          Int dof,
                          Int s)
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMDACreate2d(comm, bx, by, stencil_type, M, N, m, n, dof, s, NULL, NULL, &dm));
    return StructuredMesh(dm);
}

} // namespace godzilla
