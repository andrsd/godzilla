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
    PetscErrorCode ierr;
    PetscInt first, last;
    ierr = DMPlexGetHeightStratum(this->dm, 0, &first, &last);
    check_petsc_error(ierr);
    return last - first;
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

void
UnstructuredMesh::output_partitioning(PetscViewer viewer)
{
    _F_;
    PetscErrorCode ierr;

    DM dm_part;
    ierr = DMClone(this->dm, &dm_part);
    check_petsc_error(ierr);

    DMSetNumFields(dm_part, 1);
    PetscSection s;
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[this->dim + 1];
    for (PetscInt i = 0; i < this->dim + 1; i++)
        if (i == this->dim)
            n_dofs[i] = 1;
        else
            n_dofs[i] = 0;
    DMPlexCreateSection(dm_part, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &s);
    PetscSectionSetFieldName(s, 0, "");
    DMSetLocalSection(dm_part, s);

    Vec local, global;
    DMCreateLocalVector(dm_part, &local);
    PetscObjectSetName((PetscObject) local, "partitioning");
    DMCreateGlobalVector(dm_part, &global);
    PetscObjectSetName((PetscObject) global, "partitioning");
    VecSet(global, 1.0);

    DMGlobalToLocalBegin(dm_part, global, INSERT_VALUES, local);
    DMGlobalToLocalEnd(dm_part, global, INSERT_VALUES, local);
    VecScale(local, get_processor_id());

    DMLocalToGlobalBegin(dm_part, local, INSERT_VALUES, global);
    DMLocalToGlobalEnd(dm_part, local, INSERT_VALUES, global);

    DMView(dm_part, viewer);
    VecView(global, viewer);

    DMDestroy(&dm_part);
}

} // namespace godzilla
