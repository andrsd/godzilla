#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "Object.h"

using namespace godzilla;

registerObject(Object);

TEST(ObjectTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = Object::valid_params();
    auto obj = app.build_object<Object>("Object", "name", params);

    EXPECT_EQ(obj->get_name(), "name");
    EXPECT_EQ(obj->get_type(), "Object");

    const auto & p = obj->get_parameters();

    EXPECT_TRUE(obj->is_param_valid("_name"));

    EXPECT_EQ(obj->get_processor_id(), 0);

    PetscMPIInt sz;
    MPI_Comm_size(app.get_comm(), &sz);
    EXPECT_EQ(obj->get_comm_size(), sz);

    obj->create();
}
