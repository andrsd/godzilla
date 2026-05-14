#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/Types.h"

using namespace godzilla;

TEST(ExodusIIOutputTest, get_file_ext)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = ExodusIIOutput::parameters();
    params.set<Ref<App>>("app", ref(app));
    params.set<Ref<Problem>>("_problem", ref(prob));
    params.set<fs::path>("file", "out");
    params.set<ExecuteOnFlags>("on", ExecuteOn::NONE);
    ExodusIIOutput out(params);
    out.create();

    EXPECT_EQ(out.get_file_name(), "out.exo");
}

TEST(ExodusIIOutputTest, create)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = app.make_parameters<ExodusIIOutput>();
    params.set<fs::path>("file", "out");
    auto out = prob.add_output<ExodusIIOutput>(params);

    out->create();
}

TEST(ExodusIIOutputTest, non_existent_var)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = app.make_parameters<ExodusIIOutput>();
    params.set<fs::path>("file", "out");
    params.set<std::vector<String>>("variables", { "asdf" });
    prob.add_output<ExodusIIOutput>(params);

    EXPECT_DEATH(prob.create(),
                 "Variable 'asdf' specified in 'variables' parameter does not exist. Typo?");
}

TEST(ExodusIIOutputTest, output)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = app.make_parameters<ExodusIIOutput>();
    params.set<fs::path>("file", "out");
    auto out = prob.add_output<ExodusIIOutput>(params);

    prob.create();

    out->output_step();
}

TEST(ExodusIIOutputTest, set_file_name)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = app.make_parameters<ExodusIIOutput>();
    params.set<fs::path>("file", "out");
    auto out = prob.add_output<ExodusIIOutput>(params);

    out->create();

    EXPECT_EQ(out->get_file_name(), "out.exo");
}

TEST(ExodusIIOutputTest, set_seq_file_name)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<Ref<App>>("app", ref(app));
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTestFENonlinearProblem prob(prob_pars);

    auto params = app.make_parameters<ExodusIIOutput>();
    params.set<fs::path>("file", "out");
    auto out = prob.add_output<ExodusIIOutput>(params);

    out->create();

    out->set_sequence_file_base(2);
    EXPECT_EQ(out->get_file_name(), "out.2.exo");
}

TEST(ExodusIIOutputTest, append)
{
    TestApp app;

    // create an initial file
    {
        auto mesh_pars = app.make_parameters<LineMesh>();
        mesh_pars.set<Int>("nx", 2);
        auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

        auto prob_pars = app.make_parameters<GTestImplicitFENonlinearProblem>();
        prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
        prob_pars.set<Real>("start_time", 0.);
        prob_pars.set<Int>("num_steps", 1);
        prob_pars.set<Real>("dt", 0.1);
        GTestImplicitFENonlinearProblem prob(prob_pars);

        auto params = app.make_parameters<ExodusIIOutput>();
        params.set<fs::path>("file", "to_append_into");
        auto out = prob.add_output<ExodusIIOutput>(params);

        prob.create();
        auto & sln = prob.get_solution_vector_local();
        sln.set_values({ 0, 1, 2 }, { 10., 11., 12. });

        out->output_step();
    }

    // append into
    {
        auto mesh_pars = app.make_parameters<LineMesh>();
        mesh_pars.set<Int>("nx", 2);
        auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

        auto prob_pars = app.make_parameters<GTestImplicitFENonlinearProblem>();
        prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
        prob_pars.set<Real>("start_time", 2.);
        prob_pars.set<Int>("num_steps", 1);
        prob_pars.set<Real>("dt", 0.1);
        GTestImplicitFENonlinearProblem prob(prob_pars);

        auto params = app.make_parameters<ExodusIIOutput>();
        params.set<fs::path>("file", "to_append_into");
        params.set<bool>("append", true);
        auto out = prob.add_output<ExodusIIOutput>(params);

        prob.create();
        auto & sln = prob.get_solution_vector_local();
        sln.set_values({ 0, 1, 2 }, { 22., 12., 9. });

        out->output_step();
    }

    // check
    {
        exodusIIcpp::File exo("to_append_into.exo", exodusIIcpp::FileAccess::READ);
        exo.read();
        auto num_ts = exo.get_num_times();
        EXPECT_EQ(num_ts, 2);

        auto vals1 = exo.get_nodal_variable_values(1, 1);
        ASSERT_EQ(vals1.size(), 3);
        EXPECT_NEAR(vals1[0], 10., 1e-10);
        EXPECT_NEAR(vals1[1], 11., 1e-10);
        EXPECT_NEAR(vals1[2], 12., 1e-10);

        auto vals2 = exo.get_nodal_variable_values(2, 1);
        ASSERT_EQ(vals2.size(), 3);
        EXPECT_NEAR(vals2[0], 22., 1e-10);
        EXPECT_NEAR(vals2[1], 12., 1e-10);
        EXPECT_NEAR(vals2[2], 9., 1e-10);
    }
}
