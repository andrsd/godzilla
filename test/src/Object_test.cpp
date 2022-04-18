#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "Object.h"

using namespace godzilla;

registerObject(Object);

TEST(ObjectTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters & params = Factory::get_valid_params("Object");
    auto obj = app.build_object<Object>("Object", "name", params);

    EXPECT_EQ(obj->getName(), "name");
    EXPECT_EQ(obj->getType(), "Object");

    const auto & p = obj->getParameters();

    EXPECT_TRUE(obj->is_param_valid("_name"));

    EXPECT_EQ(obj->processorId(), 0);

    EXPECT_EQ(obj->comm(), MPI_COMM_WORLD);

    PetscMPIInt sz;
    MPI_Comm_size(app.get_comm(), &sz);
    EXPECT_EQ(obj->commSize(), sz);

    EXPECT_EQ(&obj->getApp(), &app);

    // for code coverage
    obj->create();
    obj->check();
}
