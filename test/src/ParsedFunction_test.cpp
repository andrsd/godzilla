#include "gmock/gmock.h"
#include "godzilla/Godzilla.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/ParsedFunction.h"

using namespace godzilla;

TEST(ParsedFunctionTest, eval)
{
    TestApp app;

    Parameters mesh_params = RectangleMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    mesh_params.set<Int>("ny") = 1;
    RectangleMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    mesh.create();
    prob.create();

    std::map<std::string, Real> consts;
    consts["Re"] = 100;

    Parameters params = ParsedFunction::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "fn1";
    params.set<std::vector<std::string>>("function") = { "t + x + y + Re" };
    params.set<std::map<std::string, Real>>("constants") = consts;
    ParsedFunction obj(params);

    Real x[] = { 2., 3. };
    Real u[] = { 0. };
    obj.evaluate(0.5, x, u);
    EXPECT_EQ(u[0], 105.5);
}

TEST(ParsedFunctionTest, multi_eval)
{
    TestApp app;

    Parameters mesh_params = RectangleMesh::parameters();
    mesh_params.set<App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    mesh_params.set<Int>("ny") = 1;
    RectangleMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app") = &app;
    prob_params.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    mesh.create();
    prob.create();

    Parameters params = ParsedFunction::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "fn1";
    params.set<std::vector<std::string>>("function") = { "x + 1", "2*t + y" };
    ParsedFunction obj(params);

    Real x[] = { 2., 3. };
    Real u[] = { 0., 0. };
    obj.evaluate(0.5, x, u);
    EXPECT_EQ(u[0], 3.);
    EXPECT_EQ(u[1], 4.);
}
