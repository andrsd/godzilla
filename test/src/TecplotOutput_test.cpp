#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/TecplotOutput.h"
#include "godzilla/LineMesh.h"

using namespace godzilla;

#ifdef GODZILLA_WITH_TECIOCPP

TEST(TecplotOutputTest, get_file_ext)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::string>("file", "out");
    TecplotOutput out(params);

    mesh.create();
    prob.create();
    out.create();

    EXPECT_EQ(out.get_file_name(), "out.szplt");
}

TEST(TecplotOutputTest, output)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::string>("file", "out");
    TecplotOutput out(params);
    prob.add_output(&out);

    mesh.create();
    prob.create();

    EXPECT_TRUE(app.check_integrity());
    app.get_logger()->print();

    out.output_step();
}

#else

TEST(TecplotOutputTest, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<std::string>("file", "out");
    TecplotOutput out(params);

    prob.add_output(&out);

    mesh.create();
    prob.create();

    testing::internal::CaptureStderr();

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("Unable to use TecplotOutput, godzilla was not built with teciocpp."));
}

#endif
