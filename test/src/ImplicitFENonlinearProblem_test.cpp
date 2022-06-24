#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "Factory.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "InputParameters.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "Output.h"
#include "petsc.h"
#include "petscvec.h"

using namespace godzilla;

TEST_F(ImplicitFENonlinearProblemTest, run)
{
    auto mesh = gMesh1d();
    auto prob = gProblem1d(mesh);
    this->app->problem = prob;

    {
        const std::string class_name = "ConstantIC";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const FEProblemInterface *>("_fepi") = prob;
        params->set<std::vector<PetscReal>>("value") = { 0 };
        auto ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        params->set<const FEProblemInterface *>("_fepi") = prob;
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    prob->run();

    const Vec x = prob->get_solution_vector();

    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);

    EXPECT_NEAR(xx[0], 0.5, 1e-7);
}

TEST_F(ImplicitFENonlinearProblemTest, wrong_scheme)
{
    testing::internal::CaptureStderr();

    auto mesh = gMesh1d();

    GTestImplicitFENonlinearProblem * prob;
    {
        const std::string class_name = "GTestImplicitFENonlinearProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<PetscReal>("start_time") = 0.;
        params->set<PetscReal>("end_time") = 20;
        params->set<PetscReal>("dt") = 5;
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
