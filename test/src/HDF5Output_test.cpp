#include "GodzillaApp_test.h"
#include "HDF5Output_test.h"
#include "petsc.h"
#include "petscviewerhdf5.h"

TEST_F(HDF5OutputTest, get_file_ext)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->getFileExt(), "h5");
}

TEST_F(HDF5OutputTest, create)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    out->create();
}

TEST_F(HDF5OutputTest, check)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    out->check();
}

TEST_F(HDF5OutputTest, set_file_name)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    out->create();
    out->setFileName();
    EXPECT_EQ(out->getFileName(), "out.h5");
}

TEST_F(HDF5OutputTest, set_seq_file_name)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    out->create();
    out->setSequenceFileName(2);
    EXPECT_EQ(out->getFileName(), "out.2.h5");
}

TEST_F(HDF5OutputTest, output)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->setFileName();
    out->outputStep(-1, grid->getDM(), prob->getSolutionVector());

    const std::string file_name = out->getFileName();
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

// TODO: write a test for output of sequence
