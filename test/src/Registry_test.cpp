#include "gtest/gtest.h"
#include "godzilla/Registry.h"
#include "godzilla/App.h"

using namespace godzilla;

namespace {

class ASDF : public Object {
public:
    explicit ASDF(const Parameters & parameters) : Object(parameters) {}
};

} // namespace

TEST(RegistryTest, exists)
{
    auto registry = Registry::instance();
    EXPECT_FALSE(registry.exists("ASDF"));

    registry.reg<ASDF>("ASDF");
    EXPECT_TRUE(registry.exists("ASDF"));
}

TEST(RegistryTest, get)
{
    auto registry = Registry::instance();
    EXPECT_DEATH(auto entry = registry.get("ASDF"), "Class 'ASDF' is not registered.");
}
