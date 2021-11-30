#include "GodzillaApp_test.h"
#include "HDF5Output_test.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

TEST_F(HDF5OutputTest, output)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setFileName();
    out1->output(grid->getDM(), prob->getSolutionVector());

    const std::string file_name = out1->getFileName();
    PetscViewer viewer;
    Vec sln;
    PetscReal diff;
    DMCreateGlobalVector(grid->getDM(), &sln);
    PetscObjectSetName((PetscObject) sln, "sln");
    PetscViewerHDF5Open(PETSC_COMM_WORLD, file_name.c_str(), FILE_MODE_READ, &viewer);
    VecLoad(sln, viewer);
    VecAXPY(sln, -1.0, prob->getSolutionVector());
    VecNorm(sln, NORM_INFINITY, &diff);
    EXPECT_LT(diff, PETSC_MACHINE_EPSILON);
    PetscViewerDestroy(&viewer);
    VecDestroy(&sln);
}

TEST_F(HDF5OutputTest, set_file_name)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setFileName();
    EXPECT_EQ(out1->getFileName(), "out.h5");
}

TEST_F(HDF5OutputTest, set_seq_file_name)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setSequenceFileName(2);
    EXPECT_EQ(out1->getFileName(), "out.2.h5");
}
