#include "gmock/gmock.h"
#include "godzilla/Godzilla.h"
#include "GodzillaApp_test.h"
#include "godzilla/ConstantFunction.h"

using namespace godzilla;

TEST(ConstantFunctionTest, fn_parser_eval)
{
    TestApp app;

    Parameters params = ConstantFunction::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "fn";
    params.set<Real>("value") = 123.;
    ConstantFunction obj(params);
    obj.create();

    mu::Parser parser;
    obj.register_callback(parser);

    parser.SetExpr("fn(0)");
    EXPECT_EQ(parser.Eval(), 123.);
}
