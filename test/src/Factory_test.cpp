#include "gtest/gtest.h"
#include "TestApp.h"
#include "godzilla/App.h"
#include "godzilla/Factory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;

namespace {

class TestObject : public Object {
public:
    explicit TestObject(const Parameters & parameters) : Object(parameters) {}
};

} // namespace

TEST(FactoryTest, get_parameters)
{
    godzilla::Registry reg;
    reg.add<TestObject>("TestObject");

    TestApp app;

    Factory factory(reg);
    auto params = factory.get_parameters("TestObject");
    params->set<App *>("_app") = &app;
    factory.create<TestObject>("name", params);
}

TEST(FactoryTest, is_registered)
{
    godzilla::Registry reg;
    reg.add<TestObject>("TestObject");
    Factory factory(reg);
    EXPECT_TRUE(factory.is_registered("TestObject"));
    EXPECT_FALSE(factory.is_registered("ASDF"));
}

TEST(FactoryTest, create_wrong_type)
{
    godzilla::Registry reg;
    reg.add<TestObject>("TestObject");
    reg.add<LineMesh>("LineMesh");

    TestApp app;
    Factory factory(reg);
    Parameters * params = factory.get_parameters("LineMesh");
    params->set<App *>("_app") = &app;
    params->set<Int>("nx") = 1;
    EXPECT_THROW_MSG(factory.create<RectangleMesh>("name", params),
                     "Instantiation of object 'name:[LineMesh]' failed.");
}

TEST(FactoryTest, create_non_existent)
{
    godzilla::Registry reg;

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, reg, "test", {});

    Factory factory(reg);
    EXPECT_THROW_MSG({ [[maybe_unused]] auto params = factory.get_parameters("TestObject"); },
                     "Class 'TestObject' is not registered.");
}
