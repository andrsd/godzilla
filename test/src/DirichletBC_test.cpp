#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "Factory.h"
#include "LineMesh.h"
#include "DirichletBC.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

TEST(DirichletBCTest, api)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = GTestFENonlinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
    app.problem = &problem;

    InputParameters params = DirichletBC::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<const DiscreteProblemInterface *>("_dpi") = &problem;
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    params.set<std::vector<std::string>>("value_t") = { "1" };
    DirichletBC obj(params);

    mesh.create();
    problem.create();
    obj.create();

    EXPECT_EQ(obj.get_num_components(), 1);
    EXPECT_EQ(obj.get_bc_type(), DM_BC_ESSENTIAL);

    PetscInt dim = 3;
    PetscReal time = 2.5;
    PetscReal x[] = { 3, 5, 7 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };

    obj.evaluate(dim, time, x, Nc, u);
    EXPECT_EQ(u[0], 37.5);

    obj.evaluate_t(dim, time, x, Nc, u);
    EXPECT_EQ(u[0], 1.);
}

TEST(DirichletBCTest, with_user_defined_fn)
{
    TestApp app;

    InputParameters mesh_pars = LineMesh::valid_params();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    InputParameters prob_pars = GTestFENonlinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
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
    bc_pars->set<const DiscreteProblemInterface *>("_dpi") = &problem;
    bc_pars->set<std::vector<std::string>>("value") = { "ipol(x)" };
    DirichletBC * bc = app.build_object<DirichletBC>("DirichletBC", "name", bc_pars);

    mesh.create();
    problem.create();
    bc->create();

    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[] = { 0.5 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    bc->evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 1.5);
}
