#include "GodzillaApp_test.h"
#include "VTKOutput_test.h"
#include "petsc.h"

TEST_F(VTKOutputTest, output)
{
    testing::internal::CaptureStderr();

    auto grid = gGrid();
    grid->create();
    auto prob = gProblem(grid);
    prob->create();

    auto out1 = gOutput(prob, "out.vtk");
    // TODO: when we have simple FE testing problem, we need to use it here
    // FE problem uses DMPLEX and the output will work. GTestPetscLinearProblem
    // uses DA and the VTK ouputer does not work with that. That's why we see
    // PETSc ERROR
    out1->output(grid->getDM(), prob->getSolutionVector());
}

TEST_F(VTKOutputTest, set_file_name)
{
    auto grid = gGrid();
    grid->create();
    auto prob = gProblem(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setFileName();
    // TODO: check that the file name was actually set
}

TEST_F(VTKOutputTest, set_seq_file_name)
{
    auto grid = gGrid();
    grid->create();
    auto prob = gProblem(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setSequenceFileName(2);
    // TODO: check that the file name was actually set
}
