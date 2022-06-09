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
    PetscErrorCode ierr;

    set_file_name();

    PetscViewer viewer;
    ierr = PetscViewerHDF5Open(get_comm(), get_file_name().c_str(), FILE_MODE_WRITE, &viewer);
    check_petsc_error(ierr);

    DM dmp;
    ierr = DMClone(this->problem->get_dm(), &dmp);
    check_petsc_error(ierr);

    PetscInt dim;
    ierr = DMGetDimension(dmp, &dim);
    check_petsc_error(ierr);

    ierr = DMSetNumFields(dmp, 1);
    check_petsc_error(ierr);
    PetscSection s;
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[dim + 1];
    for (PetscInt i = 0; i < dim + 1; i++)
        if (i == dim)
            n_dofs[i] = 1;
        else
            n_dofs[i] = 0;
    ierr = DMPlexCreateSection(dmp, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &s);
    check_petsc_error(ierr);
    ierr = PetscSectionSetFieldName(s, 0, "");
    check_petsc_error(ierr);
    ierr = DMSetLocalSection(dmp, s);
    check_petsc_error(ierr);

    Vec local, global;
    ierr = DMCreateLocalVector(dmp, &local);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) local, "partitioning");
    check_petsc_error(ierr);
    ierr = DMCreateGlobalVector(dmp, &global);
    check_petsc_error(ierr);
    ierr = PetscObjectSetName((PetscObject) global, "partitioning");
    check_petsc_error(ierr);
    ierr = VecSet(global, 1.0);
    check_petsc_error(ierr);

    ierr = DMGlobalToLocalBegin(dmp, global, INSERT_VALUES, local);
    check_petsc_error(ierr);
    ierr = DMGlobalToLocalEnd(dmp, global, INSERT_VALUES, local);
    check_petsc_error(ierr);
    ierr = VecScale(local, get_processor_id());
    check_petsc_error(ierr);

    ierr = DMLocalToGlobalBegin(dmp, local, INSERT_VALUES, global);
    check_petsc_error(ierr);
    ierr = DMLocalToGlobalEnd(dmp, local, INSERT_VALUES, global);
    check_petsc_error(ierr);

    ierr = DMView(dmp, viewer);
    check_petsc_error(ierr);
    ierr = VecView(global, viewer);
    check_petsc_error(ierr);

    ierr = DMDestroy(&dmp);
    check_petsc_error(ierr);

    ierr = PetscViewerDestroy(&viewer);
    check_petsc_error(ierr);
}

} // namespace godzilla
