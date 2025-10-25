// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Partitioner.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"
#include "godzilla/Exception.h"

namespace godzilla {

Partitioner::Partitioner() : PetscObjectWrapper(nullptr)
{
    CALL_STACK_MSG();
}

Partitioner::Partitioner(MPI_Comm comm) : PetscObjectWrapper(nullptr)
{
    CALL_STACK_MSG();
    create(comm);
}

Partitioner::Partitioner(PetscPartitioner p) : PetscObjectWrapper(p)
{
    CALL_STACK_MSG();
}

void
Partitioner::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerCreate(comm, &this->obj));
}

void
Partitioner::destroy()
{
    CALL_STACK_MSG();
}

void
Partitioner::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerSetType(this->obj, type.c_str()));
}

std::string
Partitioner::get_type() const
{
    CALL_STACK_MSG();
    PetscPartitionerType name;
    PETSC_CHECK(PetscPartitionerGetType(this->obj, &name));
    return { name };
}

void
Partitioner::reset()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerReset(this->obj));
}

void
Partitioner::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerSetUp(this->obj));
}

void
Partitioner::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscPartitionerView(this->obj, viewer));
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
    PETSC_CHECK(PetscPartitionerPartition(this->obj,
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
    PETSC_CHECK(PetscPartitionerPartition(this->obj,
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
    PETSC_CHECK(PetscPartitionerPartition(this->obj,
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

} // namespace godzilla
