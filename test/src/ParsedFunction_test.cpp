#include "gmock/gmock.h"
#include "Godzilla.h"
#include "GodzillaApp_test.h"
#include "ParsedFunction.h"

using namespace godzilla;

TEST(ParsedFunctionTest, fn_eval)
{
    TestApp app;

    Parameters params = ParsedFunction::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "fn1";
    params.set<std::vector<std::string>>("function") = { "3421" };
    ParsedFunction obj(params);

    PetscFunc * fn = obj.get_function();
    EXPECT_EQ(obj.get_context(), &obj);

    PetscReal x[] = { 2. };
    PetscReal u[] = { 0. };
    EXPECT_EQ((*fn)(1, 0., x, 1, u, &obj), 0);
    EXPECT_DOUBLE_EQ(u[0], 3421.);
}

TEST(ParsedFunctionTest, eval)
{
    TestApp app;

    std::map<std::string, PetscReal> consts;
    consts["Re"] = 100;

    Parameters params = ParsedFunction::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "fn1";
    params.set<std::vector<std::string>>("function") = { "t + x + y + Re" };
    params.set<std::map<std::string, PetscReal>>("constants") = consts;
    ParsedFunction obj(params);

    PetscReal x[] = { 2., 3. };
    PetscReal u[] = { 0. };
    obj.evaluate(2, 0.5, x, 1, u);
    EXPECT_EQ(105.5, u[0]);
}

TEST(ParsedFunctionTest, multi_eval)
{
    TestApp app;

    Parameters params = ParsedFunction::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "fn1";
    params.set<std::vector<std::string>>("function") = { "x + 1", "2*t + y" };
    ParsedFunction obj(params);

    PetscReal x[] = { 2., 3. };
    PetscReal u[] = { 0., 0. };
    obj.evaluate(2, 0.5, x, 2, u);
    EXPECT_EQ(3., u[0]);
    EXPECT_EQ(4., u[1]);
}

TEST(ParsedFunctionTest, eval_via_parser)
{
    TestApp app;

    Parameters params = ParsedFunction::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "exact_fn";
    params.set<std::vector<std::string>>("function") = { "t + x + y + z" };
    ParsedFunction obj(params);

    mu::Parser parser;
    obj.register_callback(parser);

    parser.SetExpr("exact_fn(t, x, y, z)");

    PetscReal time = 1;
    PetscReal xx[3] = { 2, 3, 4 };
    parser.DefineVar("t", &time);
    parser.DefineVar("x", &(xx[0]));
    parser.DefineVar("y", &(xx[1]));
    parser.DefineVar("z", &(xx[2]));

    int n_num;
    mu::value_type * val = parser.Eval(n_num);
    EXPECT_EQ(1, n_num);
    EXPECT_DOUBLE_EQ(10., val[0]);
}
