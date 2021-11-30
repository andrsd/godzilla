#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "Object.h"

using namespace godzilla;

registerObject(Object);

TEST(ObjectTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters & params = Factory::getValidParams("Object");
    auto obj = app.buildObject<Object>("Object", "name", params);

    EXPECT_EQ(obj->getName(), "name");
    EXPECT_EQ(obj->getType(), "Object");

    const auto & p = obj->getParameters();

    EXPECT_TRUE(obj->isParamValid("_name"));

    EXPECT_EQ(obj->processorId(), 0);
}
