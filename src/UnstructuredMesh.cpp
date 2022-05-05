#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

InputParameters
UnstructuredMesh::validParams()
{
    InputParameters params = Mesh::validParams();
    return params;
}

UnstructuredMesh::UnstructuredMesh(const InputParameters & parameters) :
    Mesh(parameters),
    partition_overlap(0)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerCreate(comm(), &this->partitioner);
    checkPetscError(ierr);
}

UnstructuredMesh::~UnstructuredMesh()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerDestroy(&this->partitioner);
    checkPetscError(ierr);
}

void
UnstructuredMesh::setPartitionerType(const std::string & type)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerSetType(this->partitioner, type.c_str());
    checkPetscError(ierr);
}

void
UnstructuredMesh::setPartitionOverlap(PetscInt overlap)
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
    checkPetscError(ierr);

    ierr = DMPlexSetPartitioner(this->dm, this->partitioner);
    checkPetscError(ierr);

    DM dm_dist = nullptr;
    ierr = DMPlexDistribute(this->dm, this->partition_overlap, NULL, &dm_dist);
    checkPetscError(ierr);
    if (dm_dist) {
        DMDestroy(&this->dm);
        this->dm = dm_dist;
    }
}

void
UnstructuredMesh::outputPartitioning(PetscViewer viewer)
{
    _F_;
    PetscErrorCode ierr;

    DM dm_part;
    ierr = DMClone(this->dm, &dm_part);
    checkPetscError(ierr);

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
    VecScale(local, processorId());

    DMLocalToGlobalBegin(dm_part, local, INSERT_VALUES, global);
    DMLocalToGlobalEnd(dm_part, local, INSERT_VALUES, global);

    DMView(dm_part, viewer);
    VecView(global, viewer);

    DMDestroy(&dm_part);
}

} // namespace godzilla
