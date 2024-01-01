#include "gtest/gtest.h"
#include "godzilla/Factory.h"
#include "godzilla/App.h"
#include "godzilla/RectangleMesh.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;

namespace {

class TestObject : public Object {
public:
    explicit TestObject(const Parameters & parameters) : Object(parameters) {}
};

REGISTER_OBJECT(TestObject);

} // namespace

TEST(FactoryTest, get_parameters)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "test");

    Factory factory;
    auto params = factory.get_parameters("TestObject");
    params->set<App *>("_app") = &app;
    factory.create<TestObject>("TestObject", "name", params);
}

TEST(FactoryTest, is_registered)
{
    Factory factory;
    EXPECT_TRUE(factory.is_registered("LineMesh"));
    EXPECT_FALSE(factory.is_registered("ASDF"));
}

TEST(FactoryTest, create_wrong_type)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "test");
    Factory factory;
    Parameters * params = factory.get_parameters("LineMesh");
    params->set<App *>("_app") = &app;
    params->set<Int>("nx") = 1;
    EXPECT_THROW_MSG(factory.create<RectangleMesh>("LineMesh", "name", params),
                     "Instantiation of object 'name:[LineMesh]' failed.");
}
