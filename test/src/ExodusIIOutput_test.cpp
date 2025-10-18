#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"

using namespace godzilla;

TEST(ExodusIIOutputTest, get_file_ext)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::string>("file", "out");
    ExodusIIOutput out(params);
    out.create();

    EXPECT_EQ(out.get_file_name(), "out.exo");
}

TEST(ExodusIIOutputTest, create)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    ExodusIIOutput out(params);

    prob.add_output(&out);
    out.create();
}

TEST(ExodusIIOutputTest, non_existent_var)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::vector<std::string>>("variables", { "asdf" });
    ExodusIIOutput out(params);

    prob.add_output(&out);
    prob.create();

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Variable 'asdf' specified in 'variables' parameter does not exist. Typo?"));
}

TEST(ExodusIIOutputTest, output)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    ExodusIIOutput out(params);
    prob.add_output(&out);

    prob.create();

    EXPECT_TRUE(app.check_integrity());
    app.get_logger()->print();

    out.output_step();
}

TEST(ExodusIIOutputTest, set_file_name)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::string>("file", "out");
    ExodusIIOutput out(params);

    out.create();

    EXPECT_EQ(out.get_file_name(), "out.exo");
}

TEST(ExodusIIOutputTest, set_seq_file_name)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::string>("file", "out");
    ExodusIIOutput out(params);

    out.create();

    out.set_sequence_file_base(2);
    EXPECT_EQ(out.get_file_name(), "out.2.exo");
}
