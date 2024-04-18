#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "godzilla/DynamicLibrary.h"
#include <filesystem>

using namespace godzilla;
using namespace testing;
namespace fs = std::filesystem;

TEST(DynamicLibraryTest, add_search_path)
{
    DynamicLibrary::clear_search_paths();
    DynamicLibrary::add_search_path(fs::path("app") / "exts");
    DynamicLibrary::add_search_path(fs::path("app") / "exts");

    auto paths = DynamicLibrary::get_search_paths();
    EXPECT_EQ(paths.size(), 1);
    EXPECT_EQ(paths[0], fs::path("app") / "exts");
}

TEST(DynamicLibraryTest, load)
{
    DynamicLibrary::clear_search_paths();
    DynamicLibrary::add_search_path(fs::path(GODZILLA_UNIT_TESTS_BINARY_DIR) / "ext");

    EXPECT_NO_THROW({
        DynamicLibrary dll("ext");
        dll.load();
        dll.unload();
    });
}

TEST(DynamicLibraryTest, symbol)
{
    DynamicLibrary::clear_search_paths();
    DynamicLibrary::add_search_path(fs::path(GODZILLA_UNIT_TESTS_BINARY_DIR) / "ext");

    EXPECT_NO_THROW({
        DynamicLibrary dll("ext");
        dll.load();

        auto add = dll.get_symbol<double(double, double)>("add");
        EXPECT_EQ(add.invoke(10, 20), 30);

        dll.unload();
    });
}

TEST(DynamicLibraryTest, non_existing_symbol)
{
    DynamicLibrary::clear_search_paths();
    DynamicLibrary::add_search_path(fs::path(GODZILLA_UNIT_TESTS_BINARY_DIR) / "ext");

    DynamicLibrary dll("ext");
    dll.load();

    EXPECT_THAT_THROW_MSG(
        { auto fn = dll.get_symbol<void()>("non-existing-fn"); },
        HasSubstr("Unable to locate 'non-existing-fn' in libext"));
}

TEST(DynamicLibraryTest, invalid_name)
{
    DynamicLibrary::clear_search_paths();
    DynamicLibrary dll("non-existing-extension");
    EXPECT_THAT_THROW_MSG({ dll.load(); }, HasSubstr("Unable to load libnon-existing-extension"));
}
