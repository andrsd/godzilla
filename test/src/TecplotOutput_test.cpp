#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/TecplotOutput.h"
#include "godzilla/LineMesh.h"

using namespace godzilla;

#ifdef GODZILLA_WITH_TECIOCPP

TEST(TecplotOutputTest, get_file_ext)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<App *>("app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<fs::path>("file", "out");
    TecplotOutput out(params);

    prob.create();
    out.create();

    EXPECT_EQ(out.get_file_name(), "out.szplt");
}

TEST(TecplotOutputTest, output)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<App *>("app", &app);
    params.set<fs::path>("file", "out");
    auto out = prob.add_output<TecplotOutput>(params);

    prob.create();

    EXPECT_TRUE(app.check_integrity());
    app.get_logger()->print();

    out->output_step();
}

#else

TEST(TecplotOutputTest, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<App *>("app", &app);
    params.set<fs::path>("file", "out");

    EXPECT_DEATH(prob.add_output<TecplotOutput>(params),
                 "Unable to use TecplotOutput, godzilla was not built with teciocpp.");
}

#endif
