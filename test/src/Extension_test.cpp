#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "godzilla/DynamicLibrary.h"
#include "godzilla/Extension.h"
#include <filesystem>

using namespace godzilla;
using namespace testing;
namespace fs = std::filesystem;

TEST(ExtensionTest, load)
{
    DynamicLibrary::clear_search_paths();
    DynamicLibrary::add_search_path(fs::path(GODZILLA_UNIT_TESTS_BINARY_DIR) / "ext");

    EXPECT_NO_THROW({
        DynamicLibrary dll("ext");
        dll.load();
        auto entry_point = dll.get_symbol<Extension *()>("extension_entry_point");
        auto ext = entry_point.invoke();
        EXPECT_EQ(ext->get_name(), "test_extension");
        dll.unload();
    });
}
