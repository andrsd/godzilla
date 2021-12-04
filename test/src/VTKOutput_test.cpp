#include "GodzillaApp_test.h"
#include "VTKOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(VTKOutputTest, get_file_ext)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->getFileExt(), "vtk");
}

TEST_F(VTKOutputTest, create)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    out->create();
}

TEST_F(VTKOutputTest, check)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();

    auto out = gOutput(prob, "out");
    out->check();
}
