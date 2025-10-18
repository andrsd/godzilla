#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/ParsedFunction.h"

using namespace godzilla;

TEST(ParsedFunctionTest, eval)
{
    TestApp app;

    auto mesh_params = RectangleMesh::parameters();
    mesh_params.set<App *>("_app", &app);
    mesh_params.set<Int>("nx", 2);
    mesh_params.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    prob.create();

    std::map<std::string, Real> consts;
    consts["Re"] = 100;

    auto params = ParsedFunction::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn1");
    params.set<std::vector<std::string>>("function", { "t + x + y + Re" });
    params.set<std::map<std::string, Real>>("constants", consts);
    ParsedFunction obj(params);

    Real x[] = { 2., 3. };
    Real u[] = { 0. };
    obj.evaluate(0.5, x, u);
    EXPECT_EQ(u[0], 105.5);
}

TEST(ParsedFunctionTest, multi_eval)
{
    TestApp app;

    auto mesh_params = RectangleMesh::parameters();
    mesh_params.set<App *>("_app", &app);
    mesh_params.set<Int>("nx", 2);
    mesh_params.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    prob.create();

    auto params = ParsedFunction::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn1");
    params.set<std::vector<std::string>>("function", { "x + 1", "2*t + y" });
    ParsedFunction obj(params);

    Real x[] = { 2., 3. };
    Real u[] = { 0., 0. };
    obj.evaluate(0.5, x, u);
    EXPECT_EQ(u[0], 3.);
    EXPECT_EQ(u[1], 4.);
}

TEST(ParsedFunctionTest, eval_via_parser)
{
    TestApp app;

    auto mesh_params = RectangleMesh::parameters();
    mesh_params.set<App *>("_app", &app);
    mesh_params.set<Int>("nx", 2);
    mesh_params.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    prob.create();

    auto params = ParsedFunction::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "exact_fn");
    params.set<std::vector<std::string>>("function", { "t + x + y" });
    ParsedFunction obj(params);

    mu::Parser parser;
    obj.register_callback(parser);

    parser.SetExpr("exact_fn(t, x, y, z)");

    Real time = 1;
    Real xx[2] = { 2, 3 };
    Real zero = 0.;
    parser.DefineVar("t", &time);
    parser.DefineVar("x", &(xx[0]));
    parser.DefineVar("y", &(xx[1]));
    parser.DefineVar("z", &zero);

    int n_num;
    mu::value_type * val = parser.Eval(n_num);
    EXPECT_EQ(1, n_num);
    EXPECT_DOUBLE_EQ(val[0], 6.);
}
