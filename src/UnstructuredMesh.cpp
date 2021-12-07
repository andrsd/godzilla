#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"
#include "petscdmplex.h"

namespace godzilla {

InputParameters
UnstructuredMesh::validParams()
{
    InputParameters params = Grid::validParams();
    return params;
}

UnstructuredMesh::UnstructuredMesh(const InputParameters & parameters) : Grid(parameters)
{
    _F_;
}

void
UnstructuredMesh::distribute(PetscInt overlap)
{
    _F_;
    PetscErrorCode ierr;
    DM dm_dist = nullptr;
    ierr = DMPlexDistribute(this->dm, overlap, NULL, &dm_dist);
    checkPetscError(ierr);
    if (dm_dist) {
        DMDestroy(&this->dm);
        this->dm = dm_dist;
    }
}

} // namespace godzilla
