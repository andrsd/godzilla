#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/ConstantFunction.h"

using namespace godzilla;

TEST(ConstantFunctionTest, fn_parser_eval)
{
    TestApp app;

    auto params = ConstantFunction::parameters();
    // clang-format off
    params
        .set<App *>("_app", &app)
        .set<std::string>("_name", "fn")
        .set<Real>("value", 123.);
    // clang-format on
    ConstantFunction obj(params);
    obj.create();

    mu::Parser parser;
    obj.register_callback(parser);

    parser.SetExpr("fn(0)");
    EXPECT_EQ(parser.Eval(), 123.);
}
