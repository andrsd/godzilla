#include "gmock/gmock.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "godzilla/Parameters.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/Output.h"

using namespace godzilla;

TEST_F(ImplicitFENonlinearProblemTest, run)
{
    {
        const std::string class_name = "ConstantInitialCondition";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<Real>>("value") = { 0 };
        auto ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<std::vector<std::string>>("boundary") = { "left", "right" };
        params->set<std::vector<std::string>>("value") = { "x*x" };
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    this->mesh->create();
    this->prob->create();

    EXPECT_EQ(prob->get_time_step(), 5.);
    EXPECT_DOUBLE_EQ(prob->get_max_time(), 20.);

    this->prob->run();

    auto x = this->prob->get_solution_vector();
    auto xx = x.get_array_read();
    EXPECT_NEAR(xx[0], 0.5, 1e-7);
    x.restore_array_read(xx);

    this->prob->compute_solution_vector_local();
    {
        auto lx = this->prob->get_solution_vector_local();
        auto lxx = lx.get_array_read();
        EXPECT_NEAR(lxx[0], 0., 1e-7);
        EXPECT_NEAR(lxx[1], 0.5, 1e-7);
        EXPECT_NEAR(lxx[2], 1., 1e-7);
        lx.restore_array_read(lxx);
    }
    {
        const auto * c_prob = this->prob;
        auto lx = c_prob->get_solution_vector_local();
        EXPECT_NEAR(lx(0), 0., 1e-7);
        EXPECT_NEAR(lx(1), 0.5, 1e-7);
        EXPECT_NEAR(lx(2), 1., 1e-7);
    }
}

TEST_F(ImplicitFENonlinearProblemTest, wrong_scheme)
{
    testing::internal::CaptureStderr();

    GTestImplicitFENonlinearProblem * prob;
    {
        const std::string class_name = "GTestImplicitFENonlinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<MeshObject *>("_mesh_obj") = this->mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 20;
        params->set<Real>("dt") = 5;
        params->set<std::string>("scheme") = "asdf";
        prob = this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);
    }

    this->mesh->create();
    prob->create();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("prob: The 'scheme' parameter can be either 'beuler' or 'cn'."));
}

TEST_F(ImplicitFENonlinearProblemTest, wrong_time_stepping_params)
{
    testing::internal::CaptureStderr();

    const std::string class_name = "GTestImplicitFENonlinearProblem";
    Parameters * params = this->app->get_parameters(class_name);
    params->set<MeshObject *>("_mesh_obj") = this->mesh;
    params->set<Real>("start_time") = 0.;
    params->set<Int>("num_steps") = 2;
    params->set<Real>("end_time") = 20;
    params->set<Real>("dt") = 5;
    params->set<std::string>("scheme") = "asdf";
    auto prob =
        this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);

    this->mesh->create();
    prob->create();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr(
            "prob: Cannot provide 'end_time' and 'num_steps' together. Specify one or the other."));
}

TEST_F(ImplicitFENonlinearProblemTest, no_time_stepping_params)
{
    testing::internal::CaptureStderr();

    const std::string class_name = "GTestImplicitFENonlinearProblem";
    Parameters * params = this->app->get_parameters(class_name);
    params->set<MeshObject *>("_mesh_obj") = this->mesh;
    params->set<Real>("start_time") = 0.;
    params->set<Real>("dt") = 5;
    params->set<std::string>("scheme") = "asdf";
    auto prob =
        this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);

    this->mesh->create();
    prob->create();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("prob: You must provide either 'end_time' or 'num_steps' parameter."));
}

TEST_F(ImplicitFENonlinearProblemTest, set_schemes)
{
    this->app->set_problem(this->prob);

    Parameters * ic_params = this->app->get_parameters("ConstantInitialCondition");
    ic_params->set<DiscreteProblemInterface *>("_dpi") = this->prob;
    ic_params->set<std::vector<Real>>("value") = { 0 };
    auto ic =
        this->app->build_object<InitialCondition>("ConstantInitialCondition", "ic", ic_params);
    prob->add_initial_condition(ic);

    Parameters * prob_params = this->app->get_parameters("DirichletBC");
    prob_params->set<std::vector<std::string>>("boundary") = { "left", "right" };
    prob_params->set<std::vector<std::string>>("value") = { "x*x" };
    prob_params->set<DiscreteProblemInterface *>("_dpi") = this->prob;
    auto bc = this->app->build_object<BoundaryCondition>("DirichletBC", "bc", prob_params);
    this->prob->add_boundary_condition(bc);

    this->mesh->create();
    this->prob->create();

    TS ts = this->prob->get_ts();
    TSType type;
    std::vector<std::string> schemes = {
        "beuler",
        "cn",
    };
    std::vector<TSType> types = { TSBEULER, TSCN };
    for (std::size_t i = 0; i < schemes.size(); i++) {
        prob->set_scheme(types[i]);
        TSGetType(ts, &type);
        EXPECT_STREQ(type, types[i]);
    }
}

TEST_F(ImplicitFENonlinearProblemTest, converged_reason)
{
    this->app->set_problem(this->prob);

    Parameters * ic_params = this->app->get_parameters("ConstantInitialCondition");
    ic_params->set<DiscreteProblemInterface *>("_dpi") = this->prob;
    ic_params->set<std::vector<Real>>("value") = { 0 };
    auto ic =
        this->app->build_object<InitialCondition>("ConstantInitialCondition", "ic", ic_params);
    this->prob->add_initial_condition(ic);

    Parameters * bc_params = this->app->get_parameters("DirichletBC");
    bc_params->set<std::vector<std::string>>("boundary") = { "left", "right" };
    bc_params->set<std::vector<std::string>>("value") = { "x*x" };
    bc_params->set<DiscreteProblemInterface *>("_dpi") = this->prob;
    auto bc = this->app->build_object<BoundaryCondition>("DirichletBC", "bc", bc_params);
    prob->add_boundary_condition(bc);

    this->mesh->create();
    this->prob->create();

    this->prob->set_converged_reason(TS_CONVERGED_USER);
    EXPECT_EQ(this->prob->get_converged_reason(), TS_CONVERGED_USER);
}
