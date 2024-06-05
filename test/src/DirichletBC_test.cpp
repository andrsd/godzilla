#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/Factory.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/LineMesh.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/PiecewiseLinear.h"

using namespace godzilla;

TEST(DirichletBCTest, api)
{
    TestApp app;

    Parameters mesh_pars = BoxMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    mesh_pars.set<Int>("ny") = 2;
    mesh_pars.set<Int>("nz") = 2;
    BoxMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    Parameters params = DirichletBC::parameters();
    params.set<App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    params.set<std::vector<std::string>>("value_t") = { "1" };
    DirichletBC obj(params);

    mesh.create();
    problem.create();
    obj.create();

    const auto & components = obj.get_components();
    EXPECT_EQ(components.size(), 1);

    Real time = 2.5;
    Real x[] = { 3, 5, 7 };
    Scalar u[] = { 0 };

    obj.evaluate(time, x, u);
    EXPECT_EQ(u[0], 37.5);

    obj.evaluate_t(time, x, u);
    EXPECT_EQ(u[0], 1.);
}

TEST(DirichletBCTest, with_user_defined_fn)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    std::string class_name = "PiecewiseLinear";
    Parameters * fn_pars = app.get_parameters(class_name);
    fn_pars->set<App *>("_app") = &app;
    fn_pars->set<std::vector<Real>>("x") = { 0., 1. };
    fn_pars->set<std::vector<Real>>("y") = { 1., 2. };
    Function * fn = app.build_object<PiecewiseLinear>(class_name, "ipol", fn_pars);
    problem.add_function(fn);

    Parameters * bc_pars = app.get_parameters("DirichletBC");
    bc_pars->set<App *>("_app") = &app;
    bc_pars->set<DiscreteProblemInterface *>("_dpi") = &problem;
    bc_pars->set<std::vector<std::string>>("value") = { "ipol(x)" };
    DirichletBC * bc = app.build_object<DirichletBC>("DirichletBC", "name", bc_pars);

    mesh.create();
    problem.create();
    bc->create();

    Real time = 0;
    Real x[] = { 0.5 };
    Scalar u[] = { 0 };
    bc->evaluate(time, x, u);

    EXPECT_EQ(u[0], 1.5);
}
