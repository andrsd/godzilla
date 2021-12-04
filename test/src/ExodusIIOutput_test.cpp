#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "ExodusIIOutput_test.h"
#include "petsc.h"
#include "petscviewer.h"

TEST_F(ExodusIIOutputTest, get_file_ext)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    EXPECT_EQ(out->getFileExt(), "exo");
}

TEST_F(ExodusIIOutputTest, create)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
}

TEST_F(ExodusIIOutputTest, check)
{
    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->check();
}

TEST_F(ExodusIIOutputTest, no_fe)
{
    testing::internal::CaptureStderr();

    auto grid = gGrid1d();
    grid->create();
    auto prob = gFEProblem1d(grid);
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

    auto grid = gGrid1d();
    grid->create();
    auto prob = gProblem1d(grid);
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
    auto grid = gGrid2d();
    grid->create();
    auto prob = gProblem2d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    prob->solve();
    EXPECT_EQ(prob->converged(), true);
    out->outputStep(-1, grid->getDM(), prob->getSolutionVector());
}

TEST_F(ExodusIIOutputTest, output_3d)
{
    auto grid = gGrid3d();
    grid->create();
    auto prob = gProblem3d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->checkIntegrity();

    prob->solve();
    EXPECT_EQ(prob->converged(), true);

    out->outputStep(-1, grid->getDM(), prob->getSolutionVector());
}

TEST_F(ExodusIIOutputTest, set_file_name)
{
    auto grid = gGrid2d();
    grid->create();
    auto prob = gProblem2d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();

    out->setFileName();
    EXPECT_EQ(out->getFileName(), "out.exo");
}

TEST_F(ExodusIIOutputTest, set_seq_file_name)
{
    auto grid = gGrid2d();
    grid->create();
    auto prob = gProblem2d(grid);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();

    out->setSequenceFileName(2);
    EXPECT_EQ(out->getFileName(), "out.2.exo");
}
