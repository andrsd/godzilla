#include "GodzillaApp_test.h"
#include "VTKOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(VTKOutputTest, output)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setFileName();
    out1->output(grid->getDM(), prob->getSolutionVector());
}

TEST_F(VTKOutputTest, set_file_name)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setFileName();
    EXPECT_EQ(out1->getFileName(), "out.vtk");
}

TEST_F(VTKOutputTest, set_seq_file_name)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out1 = gOutput(prob, "out");
    out1->setSequenceFileName(2);
    EXPECT_EQ(out1->getFileName(), "out.2.vtk");
}
