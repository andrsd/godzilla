#include "GodzillaApp_test.h"
#include "VTKOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(VTKOutputTest, get_file_ext)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->getFileExt(), "vtk");
}

TEST_F(VTKOutputTest, create)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    prob->addOutput(out);
    prob->create();
}

TEST_F(VTKOutputTest, check)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();

    auto out = gOutput(prob, "out");
    out->check();
}

TEST_F(VTKOutputTest, output_1d_step)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    prob->solve();
    EXPECT_EQ(prob->converged(), true);
    out->outputStep(0, mesh->getDM(), prob->getSolutionVector());
}
