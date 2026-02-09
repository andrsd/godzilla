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
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<Ref<App>>("app", ref(app));
    params.set<Ref<Problem>>("_problem", ref(prob));
    params.set<fs::path>("file", "out");
    params.set<ExecuteOnFlags>("on", ExecuteOn::NONE);
    TecplotOutput out(params);

    prob.create();
    out.create();

    EXPECT_EQ(out.get_file_name(), "out.szplt");
}

TEST(TecplotOutputTest, output)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<Ref<App>>("app", ref(app));
    params.set<fs::path>("file", "out");
    auto out = prob.add_output<TecplotOutput>(params);

    prob.create();

    out->output_step();
}

#else

TEST(TecplotOutputTest, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<App>>("app", ref(app));
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = TecplotOutput::parameters();
    params.set<Ref<App>>("app", ref(app));
    params.set<fs::path>("file", "out");

    EXPECT_DEATH(prob.add_output<TecplotOutput>(params),
                 "Unable to use TecplotOutput, godzilla was not built with teciocpp.");
}

#endif
