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

    auto mesh_pars = BoxMesh::parameters();
    // clang-format off
    mesh_pars
        .set<App *>("_app", &app)
        .set<Int>("nx", 2)
        .set<Int>("ny", 2)
        .set<Int>("nz", 2);
    // clang-format on
    BoxMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = DirichletBC::parameters();
    params.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &problem)
        .set<std::vector<std::string>>("value", { "t * (x + y + z)" })
        .set<std::vector<std::string>>("value_t", { "1" });
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto fn_pars = PiecewiseLinear::parameters();
    fn_pars.set<App *>("_app", &app)
        .set<std::string>("_name", "ipol")
        .set<std::vector<Real>>("x", { 0., 1. })
        .set<std::vector<Real>>("y", { 1., 2. });
    PiecewiseLinear fn(fn_pars);
    problem.add_function(&fn);

    auto bc_pars = DirichletBC::parameters();
    bc_pars.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &problem)
        .set<std::vector<std::string>>("value", { "ipol(x)" });
    DirichletBC bc(bc_pars);

    mesh.create();
    problem.create();
    bc.create();

    Real time = 0;
    Real x[] = { 0.5 };
    Scalar u[] = { 0 };
    bc.evaluate(time, x, u);

    EXPECT_EQ(u[0], 1.5);
}
