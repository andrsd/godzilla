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

    InputParameters & params = Factory::getValidParams("DirichletBC");
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    auto obj = app.buildObject<DirichletBC>("DirichletBC", "name", params);
    obj->create();

    EXPECT_EQ(obj->getFieldId(), 0);
    EXPECT_EQ(obj->getNumComponents(), 1);
    EXPECT_EQ(obj->getBcType(), DM_BC_ESSENTIAL);

    PetscInt dim = 3;
    PetscReal time = 2.5;
    PetscReal x[] = { 3, 5, 7 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    obj->evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 37.5);
}

TEST(DirichletBCTest, with_user_defined_fn)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::validParams();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = GTestProblem::validParams();
    prob_pars.set<const App *>("_app") = &app;
    GTestProblem problem(prob_pars);
    app.problem = &problem;

    std::string class_name = "PiecewiseLinear";
    InputParameters & fn_pars = Factory::getValidParams(class_name);
    fn_pars.set<const App *>("_app") = &app;
    fn_pars.set<std::vector<PetscReal>>("x") = { 0., 1. };
    fn_pars.set<std::vector<PetscReal>>("y") = { 1., 2. };
    Function * fn = app.buildObject<PiecewiseLinear>(class_name, "ipol", fn_pars);
    problem.addFunction(fn);

    InputParameters & bc_pars = Factory::getValidParams("DirichletBC");
    bc_pars.set<const App *>("_app") = &app;
    bc_pars.set<std::vector<std::string>>("value") = { "ipol(x)" };
    DirichletBC * bc = app.buildObject<DirichletBC>("DirichletBC", "name", bc_pars);
    bc->create();

    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[] = { 0.5 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    bc->evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 1.5);
}
