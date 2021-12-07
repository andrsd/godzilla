#include "gmock/gmock.h"
#include "Godzilla.h"
#include "GodzillaApp_test.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

TEST(PiecewiseLinearTest, eval)
{
    TestApp app;

    InputParameters params = PiecewiseLinear::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "ipol";
    params.set<std::vector<PetscReal>>("x") = { 1., 2., 3. };
    params.set<std::vector<PetscReal>>("y") = { 3., 1., 2. };
    PiecewiseLinear obj(params);

    mu::Parser parser;
    obj.registerCallback(parser);

    parser.SetExpr("ipol(0)");
    EXPECT_EQ(parser.Eval(), 3.);
}
