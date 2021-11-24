#include "Godzilla.h"
#include "PiecewiseLinear_test.h"

using namespace godzilla;

TEST_F(PiecewiseLinearTest, eval)
{
    std::vector<PetscReal> x = { 1., 2., 3. };
    std::vector<PetscReal> y = { 3., 1., 2. };
    auto obj = buildPiecewiseLinear("ipol", x, y);

    mu::Parser parser;
    obj->registerCallback(parser);

    parser.SetExpr("ipol(0)");
    EXPECT_EQ(parser.Eval(), 3.);
}
