#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "Object.h"


using namespace godzilla;

registerObject(Object);

TEST(Object, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = Factory::getValidParams("Object");
    params.set<const App *>("_app") = &app;
    auto obj = Factory::create<Object>("Object", "name", params);

    EXPECT_EQ(obj->getName(), "name");
    EXPECT_EQ(obj->getType(), "Object");

    const auto & p = obj->getParameters();

    EXPECT_TRUE(obj->isParamValid("_name"));

    EXPECT_EQ(obj->processorId(), 0);
}
