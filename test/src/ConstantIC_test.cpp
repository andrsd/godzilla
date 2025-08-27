#include "gtest/gtest.h"
#include "TestApp.h"
#include "godzilla/Factory.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/Types.h"

using namespace godzilla;

TEST(ConstantICTest, api)
{
    TestApp app;

    auto params = ConstantInitialCondition::parameters();
    params.set<App *>("_app", &app);
    params.set<std::vector<Real>>("value", { 3, 4, 5 });
    ConstantInitialCondition obj(params);

    EXPECT_EQ(obj.get_field_id(), FieldID::INVALID);
    EXPECT_EQ(obj.get_num_components(), 3);

    Real time = 0.;
    Real x[] = { 0 };
    Scalar u[] = { 0, 0, 0 };
    obj.evaluate(time, x, u);

    EXPECT_EQ(u[0], 3);
    EXPECT_EQ(u[1], 4);
    EXPECT_EQ(u[2], 5);
}
