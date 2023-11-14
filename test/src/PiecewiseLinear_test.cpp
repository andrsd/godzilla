#include "gmock/gmock.h"
#include "Godzilla.h"
#include "GodzillaApp_test.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

TEST(PiecewiseLinearTest, eval)
{
    TestApp app;

    Parameters params = PiecewiseLinear::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "ipol";
    params.set<std::vector<Real>>("x") = { 1., 2., 3. };
    params.set<std::vector<Real>>("y") = { 3., 1., 2. };
    PiecewiseLinear obj(params);

    mu::Parser parser;
    obj.register_callback(parser);

    parser.SetExpr("ipol(0)");
    EXPECT_EQ(parser.Eval(), 3.);
}

TEST(PiecewiseLinearTest, check)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters params = PiecewiseLinear::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "ipol";
    params.set<std::vector<Real>>("x") = { 1., 2. };
    params.set<std::vector<Real>>("y") = { 3., 1., 2. };
    PiecewiseLinear obj(params);
    obj.check();

    app.check_integrity();
    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("[ERROR] ipol: Size of 'x' (2) does not match size of 'y' (3)."));
}
