#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/PiecewiseConstant.h"

using namespace godzilla;

TEST(PiecewiseConstantTest, fn_parser_eval)
{
    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::vector<Real>>("x", { 1., 2. });
    params.set<std::vector<Real>>("y", { 3., 0., -1. });
    PiecewiseConstant obj(params);

    mu::Parser parser;
    obj.register_callback(parser);

    parser.SetExpr("fn(0)");
    EXPECT_EQ(parser.Eval(), 3.);
}

TEST(PiecewiseConstantTest, left_cont_eval)
{
    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::string>("continuity", "left");
    params.set<std::vector<Real>>("x", { 1., 2., 3. });
    params.set<std::vector<Real>>("y", { 3., 0., -1., 2. });
    PiecewiseConstant obj(params);
    obj.create();

    EXPECT_EQ(obj.evaluate(0.), 3.);
    EXPECT_EQ(obj.evaluate(1.), 3.);
    EXPECT_EQ(obj.evaluate(1.5), 0.);
    EXPECT_EQ(obj.evaluate(2.), 0.);
    EXPECT_EQ(obj.evaluate(2.5), -1.);
    EXPECT_EQ(obj.evaluate(3.), -1.);
    EXPECT_EQ(obj.evaluate(3.5), 2.);
}

TEST(PiecewiseConstantTest, right_cont_eval)
{
    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::string>("continuity", "right");
    params.set<std::vector<Real>>("x", { 1., 2., 3. });
    params.set<std::vector<Real>>("y", { 3., 0., -1., 2. });
    PiecewiseConstant obj(params);
    obj.create();

    EXPECT_EQ(obj.evaluate(0.), 3.);
    EXPECT_EQ(obj.evaluate(1.), 0.);
    EXPECT_EQ(obj.evaluate(1.5), 0.);
    EXPECT_EQ(obj.evaluate(2.), -1.);
    EXPECT_EQ(obj.evaluate(2.5), -1.);
    EXPECT_EQ(obj.evaluate(3.), 2.);
    EXPECT_EQ(obj.evaluate(3.5), 2.);
}

TEST(PiecewiseConstantTest, err_incorrect_point_count)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::vector<Real>>("x", { 1., 2. });
    params.set<std::vector<Real>>("y", { 3., 1. });
    PiecewiseConstant obj(params);
    obj.create();

    app.check_integrity();
    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("[ERROR] fn: Size of 'x' (2) does not match size of 'y' (2)."));
}

TEST(PiecewiseConstantTest, err_no_points)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::vector<Real>>("x", {});
    params.set<std::vector<Real>>("y", { 1. });
    PiecewiseConstant obj(params);
    obj.create();

    app.check_integrity();
    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("[ERROR] fn: Size of 'x' is 0. It must be 1 or more."));
}

TEST(PiecewiseConstantTest, err_not_monotonic)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::vector<Real>>("x", { 1., 0. });
    params.set<std::vector<Real>>("y", { 1., 2., 3. });
    PiecewiseConstant obj(params);
    obj.create();

    app.check_integrity();
    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("[ERROR] fn: Values in 'x' must be increasing. Failed at index '1'."));
}

TEST(PiecewiseConstantTest, err_cont)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = PiecewiseConstant::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "fn");
    params.set<std::string>("continuity", "asdf");
    params.set<std::vector<Real>>("x", { 1. });
    params.set<std::vector<Real>>("y", { 1., 2. });
    PiecewiseConstant obj(params);
    obj.create();

    app.check_integrity();
    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "[ERROR] fn: The 'continuity' parameter can be either 'left' or 'right'."));
}
