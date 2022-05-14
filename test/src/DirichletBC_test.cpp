#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "GYMLFile_test.h"
#include "Factory.h"
#include "LineMesh.h"
#include "DirichletBC.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

TEST(DirichletBCTest, api)
{
    TestApp app;

    InputParameters params = DirichletBC::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    DirichletBC obj(params);
    obj.create();

    EXPECT_EQ(obj.get_field_id(), 0);
    EXPECT_EQ(obj.get_num_components(), 1);
    EXPECT_EQ(obj.get_bc_type(), DM_BC_ESSENTIAL);

    PetscInt dim = 3;
    PetscReal time = 2.5;
    PetscReal x[] = { 3, 5, 7 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    obj.evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 37.5);
}

TEST(DirichletBCTest, with_user_defined_fn)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = GTestProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    GTestProblem problem(prob_pars);
    app.problem = &problem;

    std::string class_name = "PiecewiseLinear";
    InputParameters * fn_pars = Factory::get_valid_params(class_name);
    fn_pars->set<const App *>("_app") = &app;
    fn_pars->set<std::vector<PetscReal>>("x") = { 0., 1. };
    fn_pars->set<std::vector<PetscReal>>("y") = { 1., 2. };
    Function * fn = app.build_object<PiecewiseLinear>(class_name, "ipol", fn_pars);
    problem.add_function(fn);

    InputParameters * bc_pars = Factory::get_valid_params("DirichletBC");
    bc_pars->set<const App *>("_app") = &app;
    bc_pars->set<std::vector<std::string>>("value") = { "ipol(x)" };
    DirichletBC * bc = app.build_object<DirichletBC>("DirichletBC", "name", bc_pars);
    bc->create();

    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[] = { 0.5 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    bc->evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 1.5);
}
