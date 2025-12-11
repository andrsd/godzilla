#include "gtest/gtest.h"
#include "TestApp.h"
#include "godzilla/Factory.h"
#include "godzilla/Object.h"

using namespace godzilla;

TEST(ObjectTest, api)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    TestApp app;
    app.get_registry().add<Object>("Object");

    auto params = Object::parameters();
    params.set<String>("_type", "Object");
    auto obj = app.build_object<Object>("name", params);

    EXPECT_EQ(obj->get_name(), "name");

    EXPECT_EQ(obj->get_processor_id(), 0);

    int sz;
    MPI_Comm_size(app.get_comm(), &sz);
    EXPECT_EQ(obj->get_comm().size(), sz);

    obj->create();
}
