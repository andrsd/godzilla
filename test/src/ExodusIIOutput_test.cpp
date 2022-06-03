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
    EXPECT_EQ(out->get_file_ext(), "exo");
}

TEST_F(ExodusIIOutputTest, create)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    prob->add_output(out);
    out->create();
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

TEST_F(ExodusIIOutputTest, output)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gFEProblem(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();
    out->check();
    this->app->check_integrity();

    out->output_step();
}

TEST_F(ExodusIIOutputTest, set_file_name)
{
    auto mesh = gMesh2d();
    mesh->create();
    auto prob = gProblem2d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();

    out->set_file_name();
    EXPECT_EQ(out->get_file_name(), "out.exo");
}

TEST_F(ExodusIIOutputTest, set_seq_file_name)
{
    auto mesh = gMesh2d();
    mesh->create();
    auto prob = gProblem2d(mesh);
    prob->create();
    auto out = gOutput(prob, "out");
    out->create();

    out->set_sequence_file_name(2);
    EXPECT_EQ(out->get_file_name(), "out.2.exo");
}
