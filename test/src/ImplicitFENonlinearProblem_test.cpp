#include "gmock/gmock.h"
#include "CallStack.h"
#include "Factory.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "Parameters.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "Output.h"
#include "petscvec.h"

using namespace godzilla;

TEST_F(ImplicitFENonlinearProblemTest, run)
{
    auto mesh = gMesh1d();
    auto prob = gProblem1d(mesh);
    this->app->problem = prob;

    {
        const std::string class_name = "ConstantIC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<Real>>("value") = { 0 };
        auto ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }

    for (auto & bnd : { "left", "right" }) {
        const std::string class_name = "DirichletBC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<std::string>("boundary") = bnd;
        params->set<std::vector<std::string>>("value") = { "x*x" };
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    EXPECT_EQ(prob->get_time_step(), 5.);

    prob->run();

    auto x = prob->get_solution_vector();
    auto xx = x.get_array_read();
    EXPECT_NEAR(xx[0], 0.5, 1e-7);
    x.restore_array_read(xx);

    auto lx = prob->get_solution_vector_local();
    lx.view();
    auto lxx = lx.get_array_read();
    EXPECT_NEAR(lxx[0], 0., 1e-7);
    EXPECT_NEAR(lxx[1], 0.5, 1e-7);
    EXPECT_NEAR(lxx[2], 1., 1e-7);
    lx.restore_array_read(lxx);
}

TEST_F(ImplicitFENonlinearProblemTest, wrong_scheme)
{
    testing::internal::CaptureStderr();

    auto mesh = gMesh1d();

    GTestImplicitFENonlinearProblem * prob;
    {
        const std::string class_name = "GTestImplicitFENonlinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 20;
        params->set<Real>("dt") = 5;
        params->set<std::string>("scheme") = "asdf";
        prob = this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);
    }

    mesh->create();
    prob->create();
    prob->check();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("prob: The 'scheme' parameter can be either 'beuler' or 'cn'."));
}

TEST_F(ImplicitFENonlinearProblemTest, wrong_time_stepping_params)
{
    testing::internal::CaptureStderr();

    auto mesh = gMesh1d();

    const std::string class_name = "GTestImplicitFENonlinearProblem";
    Parameters * params = Factory::get_parameters(class_name);
    params->set<Mesh *>("_mesh") = mesh;
    params->set<Real>("start_time") = 0.;
    params->set<Int>("num_steps") = 2;
    params->set<Real>("end_time") = 20;
    params->set<Real>("dt") = 5;
    params->set<std::string>("scheme") = "asdf";
    auto prob =
        this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);

    mesh->create();
    prob->create();
    prob->check();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr(
            "prob: Cannot provide 'end_time' and 'num_steps' together. Specify one or the other."));
}

TEST_F(ImplicitFENonlinearProblemTest, no_time_stepping_params)
{
    testing::internal::CaptureStderr();

    auto mesh = gMesh1d();

    const std::string class_name = "GTestImplicitFENonlinearProblem";
    Parameters * params = Factory::get_parameters(class_name);
    params->set<Mesh *>("_mesh") = mesh;
    params->set<Real>("start_time") = 0.;
    params->set<Real>("dt") = 5;
    params->set<std::string>("scheme") = "asdf";
    auto prob =
        this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);

    mesh->create();
    prob->create();
    prob->check();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("prob: You must provide either 'end_time' or 'num_steps' parameter."));
}
