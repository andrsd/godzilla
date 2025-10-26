#include "gmock/gmock.h"
#include "godzilla/Registry.h"
#include "godzilla/App.h"
#include "godzilla/Types.h"
#include "godzilla/LineMesh.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;

namespace {

class ASDF : public Object {
public:
    explicit ASDF(const Parameters & pars) : Object(pars) {}

    static Parameters parameters();
};

Parameters
ASDF::parameters()
{
    auto params = Object::parameters();
    params.add_required_param<Int>("required_value", "Required value")
        .add_param<Real>("value_with_default", 0., "Some value")
        .add_param<bool>("on", false, "On/Off switch")
        .add_private_param<bool>("_private_value", false)
        .add_required_param<std::string>("str", "Text parameter [-]");
    return params;
}

} // namespace

TEST(RegistryTest, exists)
{
    Registry registry;
    EXPECT_FALSE(registry.exists("ASDF"));

    registry.add<ASDF>("ASDF");
    EXPECT_TRUE(registry.exists("ASDF"));
}

TEST(RegistryTest, get)
{
    Registry registry;
    EXPECT_THROW_MSG([[maybe_unused]] auto entry = registry.get("ASDF"),
                     "Class 'ASDF' is not registered.");
}

TEST(RegistryTest, get_object_description)
{
    Registry registry;
    registry.add<ASDF>("ASDF");
    registry.add<LineMesh>("LineMesh");

    auto objs = registry.get_object_description();
    EXPECT_EQ(objs.size(), 2);

    auto & o0 = objs[0];
    EXPECT_EQ(o0.name, "ASDF");
    auto & p0 = o0.parameters;
    EXPECT_EQ(p0.size(), 5);
    std::vector<std::string> names;
    for (auto & par : p0)
        names.push_back(par.name);
    EXPECT_THAT(
        names,
        testing::UnorderedElementsAre("name", "required_value", "value_with_default", "on", "str"));

    auto & o1 = objs[1];
    EXPECT_EQ(o1.name, "LineMesh");
    auto & p1 = o1.parameters;
    EXPECT_EQ(p1.size(), 4);
    names.clear();
    for (auto & par : p1)
        names.push_back(par.name);
    EXPECT_THAT(names, testing::UnorderedElementsAre("name", "nx", "xmax", "xmin"));
}
