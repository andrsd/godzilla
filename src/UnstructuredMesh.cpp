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
    PetscErrorCode ierr;
    ierr = PetscPartitionerCreate(get_comm(), &this->partitioner);
    check_petsc_error(ierr);
}

UnstructuredMesh::~UnstructuredMesh()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerDestroy(&this->partitioner);
    check_petsc_error(ierr);
}

PetscInt
UnstructuredMesh::get_num_vertices() const
{
    _F_;
    PetscErrorCode ierr;
    PetscInt first, last;
    ierr = DMPlexGetHeightStratum(this->dm, this->dim, &first, &last);
    check_petsc_error(ierr);
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
    PetscErrorCode ierr;
    ierr = DMPlexGetHeightStratum(this->dm, 0, &first, &last);
    check_petsc_error(ierr);
}

void
UnstructuredMesh::set_partitioner_type(const std::string & type)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerSetType(this->partitioner, type.c_str());
    check_petsc_error(ierr);
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
    PetscErrorCode ierr;

    ierr = PetscPartitionerSetUp(this->partitioner);
    check_petsc_error(ierr);

    ierr = DMPlexSetPartitioner(this->dm, this->partitioner);
    check_petsc_error(ierr);

    DM dm_dist = nullptr;
    ierr = DMPlexDistribute(this->dm, this->partition_overlap, NULL, &dm_dist);
    check_petsc_error(ierr);
    if (dm_dist) {
        DMDestroy(&this->dm);
        this->dm = dm_dist;
    }
}

bool
UnstructuredMesh::is_simplex() const
{
    _F_;
    PetscBool simplex;
    PetscErrorCode ierr = DMPlexIsSimplex(this->dm, &simplex);
    check_petsc_error(ierr);
    return simplex == PETSC_TRUE;
}

} // namespace godzilla
