#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "Object.h"

using namespace godzilla;

REGISTER_OBJECT(Object);

TEST(ObjectTest, api)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "test");

    Parameters params = Object::parameters();
    auto obj = app.build_object<Object>("Object", "name", params);

    EXPECT_EQ(obj->get_name(), "name");
    EXPECT_EQ(obj->get_type(), "Object");

    const auto & p = obj->get_parameters();

    EXPECT_TRUE(obj->is_param_valid("_name"));

    EXPECT_EQ(obj->get_processor_id(), 0);

    int sz;
    MPI_Comm_size(app.get_comm(), &sz);
    EXPECT_EQ(obj->get_comm().size(), sz);

    obj->create();
}
