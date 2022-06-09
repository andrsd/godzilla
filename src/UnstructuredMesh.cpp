#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

InputParameters
UnstructuredMesh::valid_params()
{
    InputParameters params = Mesh::valid_params();
    return params;
}

UnstructuredMesh::UnstructuredMesh(const InputParameters & parameters) :
    Mesh(parameters),
    partition_overlap(0)
{
    _F_;
    PETSC_CHECK(PetscPartitionerCreate(get_comm(), &this->partitioner));
}

UnstructuredMesh::~UnstructuredMesh()
{
    _F_;
    PETSC_CHECK(PetscPartitionerDestroy(&this->partitioner));
}

PetscInt
UnstructuredMesh::get_num_vertices() const
{
    _F_;
    PetscInt first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, this->dim, &first, &last));
    return last - first;
}

PetscInt
UnstructuredMesh::get_num_elements() const
{
    _F_;
    PetscInt first, last;
    get_element_idx_range(first, last);
    return last - first;
}

void
UnstructuredMesh::get_element_idx_range(PetscInt & first, PetscInt & last) const
{
    _F_;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, &first, &last));
}

void
UnstructuredMesh::set_partitioner_type(const std::string & type)
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetType(this->partitioner, type.c_str()));
}

void
UnstructuredMesh::set_partition_overlap(PetscInt overlap)
{
    _F_;
    this->partition_overlap = overlap;
}

void
UnstructuredMesh::distribute()
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetUp(this->partitioner));

    PETSC_CHECK(DMPlexSetPartitioner(this->dm, this->partitioner));

    DM dm_dist = nullptr;
    PETSC_CHECK(DMPlexDistribute(this->dm, this->partition_overlap, NULL, &dm_dist));
    if (dm_dist) {
        PETSC_CHECK(DMDestroy(&this->dm));
        this->dm = dm_dist;
    }
}

bool
UnstructuredMesh::is_simplex() const
{
    _F_;
    PetscBool simplex;
    PETSC_CHECK(DMPlexIsSimplex(this->dm, &simplex));
    return simplex == PETSC_TRUE;
}

} // namespace godzilla
