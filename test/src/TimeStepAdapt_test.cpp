#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/TimeStepAdapt.h"
#include "godzilla/LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"

using namespace testing;
using namespace godzilla;

TEST(TimeStepAdapt, test)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_param = GTestImplicitFENonlinearProblem::parameters();
    prob_param.set<MeshObject *>("_mesh_obj") = &mesh;
    prob_param.set<godzilla::App *>("_app") = &app;
    prob_param.set<Real>("start_time") = 0.;
    prob_param.set<Real>("end_time") = 20;
    prob_param.set<Real>("dt") = 5;
    GTestImplicitFENonlinearProblem prob(prob_param);

    auto ts = prob.get_ts();
    auto adapt = TimeStepAdapt::from_ts(ts);

    adapt.set_clip(0.01, 1.);
    auto [low, high] = adapt.get_clip();
    EXPECT_DOUBLE_EQ(low, 0.01);
    EXPECT_DOUBLE_EQ(high, 1.);

    adapt.set_max_ignore(100.);
    EXPECT_DOUBLE_EQ(adapt.get_max_ignore(), 100.);

    // NOTE: no getter in PETSc
    adapt.set_always_accept(true);

    adapt.set_type(TimeStepAdapt::NONE);
    EXPECT_EQ(adapt.get_type(), TSADAPTNONE);

    adapt.set_safety(0.1234, 0.9876);
    auto [safety, safety_reject] = adapt.get_safety();
    EXPECT_DOUBLE_EQ(safety, 0.1234);
    EXPECT_DOUBLE_EQ(safety_reject, 0.9876);

    adapt.set_scale_solve_failed(0.5678);
    EXPECT_DOUBLE_EQ(adapt.get_scale_solve_failed(), 0.5678);

    adapt.set_step_limits(0.5, 5.123);
    auto [hmin, hmax] = adapt.get_step_limits();
    EXPECT_DOUBLE_EQ(hmin, 0.5);
    EXPECT_DOUBLE_EQ(hmax, 5.123);

    adapt.set_type(TimeStepAdapt::BASIC);
    adapt.add_candidate(TSADAPTBASIC, 1, 1, 0.5, 1., true);
    adapt.add_candidate(TSADAPTBASIC, 1, 1, 0.25, 2., false);

    auto candidates = adapt.get_candidates();
    ASSERT_EQ(candidates.size(), 2);
    EXPECT_EQ(candidates[0].order, 1);
    EXPECT_EQ(candidates[0].stage_order, 1);
    EXPECT_DOUBLE_EQ(candidates[0].ccfl, 0.5);
    EXPECT_DOUBLE_EQ(candidates[0].cost, 1.);

    EXPECT_EQ(candidates[1].order, 1);
    EXPECT_EQ(candidates[1].stage_order, 1);
    EXPECT_DOUBLE_EQ(candidates[1].ccfl, 0.25);
    EXPECT_DOUBLE_EQ(candidates[1].cost, 2.);

    auto X = prob.get_solution();
    auto accept_stage = adapt.check_stage(0., X);
    EXPECT_TRUE(accept_stage);

    auto [next_sc, next_h, accept] = adapt.choose(0.5);
    EXPECT_EQ(next_sc, 0);
    EXPECT_DOUBLE_EQ(next_h, 0.5);
    EXPECT_TRUE(accept);

    adapt.clear_candidates();

    // NOTE: for code coverage, needs a check
    adapt.reset();

    // NOTE: for code coverage, needs a check
    TimeStepAdapt empty;
}
