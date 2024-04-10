#include "gtest/gtest.h"
#include "godzilla/Registry.h"
#include "godzilla/App.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;

namespace {

class ASDF : public Object {
public:
    explicit ASDF(const Parameters & parameters) : Object(parameters) {}
};

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
    EXPECT_THROW_MSG(auto entry = registry.get("ASDF"), "Class 'ASDF' is not registered.");
}
