#include "godzilla/Partitioner.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

using namespace godzilla;

Partitioner::Partitioner() : part(nullptr)
{
    _F_;
}

Partitioner::Partitioner(MPI_Comm comm) : part(nullptr)
{
    _F_;
    create(comm);
}

Partitioner::Partitioner(PetscPartitioner p) : part(p)
{
    _F_;
}

void
Partitioner::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(PetscPartitionerCreate(comm, &this->part));
}

void
Partitioner::destroy()
{
    _F_;
    if (this->part)
        PETSC_CHECK(PetscPartitionerDestroy(&this->part));
    this->part = nullptr;
}

void
Partitioner::set_type(const std::string & type)
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetType(this->part, type.c_str()));
}

std::string
Partitioner::get_type() const
{
    _F_;
    PetscPartitionerType name;
    PETSC_CHECK(PetscPartitionerGetType(this->part, &name));
    return { name };
}

void
Partitioner::reset()
{
    _F_;
    PETSC_CHECK(PetscPartitionerReset(this->part));
}

void
Partitioner::set_up()
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetUp(this->part));
}

void
Partitioner::view(PetscViewer viewer) const
{
    _F_;
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
    _F_;
    PETSC_CHECK(PetscPartitionerPartition(this->part,
                                          n_parts,
                                          n_vertices,
                                          start,
                                          adjacency,
                                          vertex_section,
                                          target_section,
                                          part_section,
                                          partition));
}

Partitioner::operator PetscPartitioner() const
{
    _F_;
    return this->part;
}
