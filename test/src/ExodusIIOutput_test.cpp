#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "ExodusIIOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(ExodusIIOutputTest, get_file_ext)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->getFileExt(), "exo");
}

TEST_F(ExodusIIOutputTest, create)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    prob->addOutput(out);
    prob->create();
}

TEST_F(ExodusIIOutputTest, check)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->check();
}

TEST_F(ExodusIIOutputTest, no_fe)
{
    testing::internal::CaptureStderr();

    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gFEProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("out: ExodusII output does not support finite element problems yet."));
}

TEST_F(ExodusIIOutputTest, output_1d)
{
    testing::internal::CaptureStderr();

    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("out: PETSc viewer does not support ExodusII output for 1D problems."));
}

TEST_F(ExodusIIOutputTest, output_2d)
{
    auto mesh = gMesh2d();
    mesh->create();
    auto prob = gProblem2d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    prob->solve();
    EXPECT_EQ(prob->converged(), true);
    out->outputStep(-1, mesh->getDM(), prob->getSolutionVector());
}

TEST_F(ExodusIIOutputTest, output_3d)
{
    auto mesh = gMesh3d();
    mesh->create();
    auto prob = gProblem3d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    prob->solve();
    EXPECT_EQ(prob->converged(), true);

    out->outputStep(-1, mesh->getDM(), prob->getSolutionVector());
}

TEST_F(ExodusIIOutputTest, set_file_name)
{
    auto mesh = gMesh2d();
    mesh->create();
    auto prob = gProblem2d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();

    out->setFileName();
    EXPECT_EQ(out->getFileName(), "out.exo");
}

TEST_F(ExodusIIOutputTest, set_seq_file_name)
{
    auto mesh = gMesh2d();
    mesh->create();
    auto prob = gProblem2d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();

    out->setSequenceFileName(2);
    EXPECT_EQ(out->getFileName(), "out.2.exo");
}
