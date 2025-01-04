// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Partitioner.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"

using namespace godzilla;

Partitioner::Partitioner() : part(nullptr)
{
    CALL_STACK_MSG();
}

Partitioner::Partitioner(MPI_Comm comm) : part(nullptr)
{
    CALL_STACK_MSG();
    create(comm);
}

Partitioner::Partitioner(PetscPartitioner p) : part(p)
{
    CALL_STACK_MSG();
}

void
Partitioner::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerCreate(comm, &this->part));
}

void
Partitioner::destroy()
{
    CALL_STACK_MSG();
    if (this->part)
        PETSC_CHECK(PetscPartitionerDestroy(&this->part));
    this->part = nullptr;
}

void
Partitioner::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerSetType(this->part, type.c_str()));
}

std::string
Partitioner::get_type() const
{
    CALL_STACK_MSG();
    PetscPartitionerType name;
    PETSC_CHECK(PetscPartitionerGetType(this->part, &name));
    return { name };
}

void
Partitioner::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerReset(this->part));
}

void
Partitioner::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerSetUp(this->part));
}

void
Partitioner::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerView(this->part, viewer));
}

void
Partitioner::partition(Int n_parts,
                       Int n_vertices,
                       Int start[],
                       Int adjacency[],
                       const Section & vertex_section,
                       const Section & target_section,
                       Section & part_section,
                       IndexSet & partition)
{
    CALL_STACK_MSG();
#if PETSC_VERSION_GE(3, 21, 0)
    PETSC_CHECK(PetscPartitionerPartition(this->part,
                                          n_parts,
                                          n_vertices,
                                          start,
                                          adjacency,
                                          vertex_section,
                                          NULL,
                                          target_section,
                                          part_section,
                                          partition));
#else
    PETSC_CHECK(PetscPartitionerPartition(this->part,
                                          n_parts,
                                          n_vertices,
                                          start,
                                          adjacency,
                                          vertex_section,
                                          target_section,
                                          part_section,
                                          partition));
#endif
}

void
Partitioner::partition(Int n_parts,
                       Int n_vertices,
                       Int start[],
                       Int adjacency[],
                       const Section & vertex_section,
                       const Section & edge_section,
                       const Section & target_section,
                       Section & part_section,
                       IndexSet & partition)
{
    CALL_STACK_MSG();
#if PETSC_VERSION_GE(3, 21, 0)
    PETSC_CHECK(PetscPartitionerPartition(this->part,
                                          n_parts,
                                          n_vertices,
                                          start,
                                          adjacency,
                                          vertex_section,
                                          edge_section,
                                          target_section,
                                          part_section,
                                          partition));
#else
    throw Exception("PETSc 3.21+ is needed for Partitioner::partition with edge_section parameter");
#endif
}

Partitioner::operator PetscPartitioner() const
{
    CALL_STACK_MSG();
    return this->part;
}
