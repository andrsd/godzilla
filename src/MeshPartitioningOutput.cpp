// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/MeshPartitioningOutput.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "petscviewerhdf5.h"

namespace godzilla {

Parameters
MeshPartitioningOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    return params;
}

MeshPartitioningOutput::MeshPartitioningOutput(const Parameters & params) : FileOutput(params)
{
    CALL_STACK_MSG();
    set_file_base("part");
}

std::string
MeshPartitioningOutput::get_file_ext() const
{
    CALL_STACK_MSG();
    return { "h5" };
}

void
MeshPartitioningOutput::output_step()
{
    CALL_STACK_MSG();
    PetscViewer viewer;
    PETSC_CHECK(PetscViewerHDF5Open(get_comm(), get_file_name().c_str(), FILE_MODE_WRITE, &viewer));

    assert(get_problem()->get_dm() != nullptr);
    DM dmp;
    PETSC_CHECK(DMClone(get_problem()->get_dm(), &dmp));

    Int dim;
    PETSC_CHECK(DMGetDimension(dmp, &dim));

    PETSC_CHECK(DMSetNumFields(dmp, 1));
    PetscSection s;
    Int nc[1] = { 1 };
    Int n_dofs[dim + 1];
    for (Int i = 0; i < dim + 1; i++)
        if (i == dim)
            n_dofs[i] = 1;
        else
            n_dofs[i] = 0;
    PETSC_CHECK(
        DMPlexCreateSection(dmp, nullptr, nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr, &s));
    PETSC_CHECK(PetscSectionSetFieldName(s, 0, ""));
    PETSC_CHECK(DMSetLocalSection(dmp, s));
    PETSC_CHECK(DMCreateDS(dmp));

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
