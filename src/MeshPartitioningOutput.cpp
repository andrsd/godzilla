#include "Godzilla.h"
#include "CallStack.h"
#include "MeshPartitioningOutput.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "petscviewerhdf5.h"

namespace godzilla {

registerObject(MeshPartitioningOutput);

InputParameters
MeshPartitioningOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

MeshPartitioningOutput::MeshPartitioningOutput(const InputParameters & params) : FileOutput(params)
{
    _F_;
    this->file_base = "part";
}

std::string
MeshPartitioningOutput::get_file_ext() const
{
    return std::string("h5");
}

void
MeshPartitioningOutput::output_step()
{
    _F_;
    set_file_name();

    PetscViewer viewer;
    PETSC_CHECK(PetscViewerHDF5Open(get_comm(), get_file_name().c_str(), FILE_MODE_WRITE, &viewer));

    DM dmp;
    PETSC_CHECK(DMClone(this->problem->get_dm(), &dmp));

    PetscInt dim;
    PETSC_CHECK(DMGetDimension(dmp, &dim));

    PETSC_CHECK(DMSetNumFields(dmp, 1));
    PetscSection s;
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[dim + 1];
    for (PetscInt i = 0; i < dim + 1; i++)
        if (i == dim)
            n_dofs[i] = 1;
        else
            n_dofs[i] = 0;
    PETSC_CHECK(DMPlexCreateSection(dmp, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &s));
    PETSC_CHECK(PetscSectionSetFieldName(s, 0, ""));
    PETSC_CHECK(DMSetLocalSection(dmp, s));

    Vec local, global;
    PETSC_CHECK(DMCreateLocalVector(dmp, &local));
    PETSC_CHECK(PetscObjectSetName((PetscObject) local, "partitioning"));
    PETSC_CHECK(DMCreateGlobalVector(dmp, &global));
    PETSC_CHECK(PetscObjectSetName((PetscObject) global, "partitioning"));
    PETSC_CHECK(VecSet(global, 1.0));

    PETSC_CHECK(DMGlobalToLocalBegin(dmp, global, INSERT_VALUES, local));
    PETSC_CHECK(DMGlobalToLocalEnd(dmp, global, INSERT_VALUES, local));
    PETSC_CHECK(VecScale(local, get_processor_id()));

    PETSC_CHECK(DMLocalToGlobalBegin(dmp, local, INSERT_VALUES, global));
    PETSC_CHECK(DMLocalToGlobalEnd(dmp, local, INSERT_VALUES, global));

    PETSC_CHECK(DMView(dmp, viewer));
    PETSC_CHECK(VecView(global, viewer));

    PETSC_CHECK(DMDestroy(&dmp));

    PETSC_CHECK(PetscViewerDestroy(&viewer));
}

} // namespace godzilla
