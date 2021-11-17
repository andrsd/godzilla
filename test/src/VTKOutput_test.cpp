#include "GodzillaApp_test.h"
#include "VTKOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(VTKOutputTest, output)
{
    auto grid = gGrid();
    grid->create();
    auto prob = gProblem(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    // TODO: when we have simple FE testing problem, we need to use it here
    // FE problem uses DMPLEX and the output will work. G1DTestPetscLinearProblem
    // uses DA and the VTK ouputer does not work with that. That's why we see
    // PETSc ERROR
    out1->setFileName();
    out1->output(grid->getDM(), prob->getSolutionVector());

    // // this does not work in PETSc yet :<(
    // const std::string file_name = out1->getFileName();
    // PetscViewer viewer;
    // Vec sln;
    // PetscReal diff;
    // DMCreateGlobalVector(grid->getDM(), &sln);
    // PetscObjectSetName((PetscObject) sln, "sln");
    // PetscViewerVTKOpen(PETSC_COMM_WORLD, file_name.c_str(), FILE_MODE_READ, &viewer);
    // VecLoad(sln, viewer);
    // VecAXPY(sln, -1.0, prob->getSolutionVector());
    // VecNorm(sln, NORM_INFINITY, &diff);
    // EXPECT_LT(diff, PETSC_MACHINE_EPSILON);
    // PetscViewerDestroy(&viewer);
    // VecDestroy(&sln);
}

TEST_F(VTKOutputTest, set_file_name)
{
    auto grid = gGrid();
    grid->create();
    auto prob = gProblem(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setFileName();
    EXPECT_EQ(out1->getFileName(), "out.vtk");
}

TEST_F(VTKOutputTest, set_seq_file_name)
{
    auto grid = gGrid();
    grid->create();
    auto prob = gProblem(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setSequenceFileName(2);
    EXPECT_EQ(out1->getFileName(), "out.2.vtk");
}
