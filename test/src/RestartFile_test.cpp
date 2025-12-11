#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/RestartFile.h"
#include "godzilla/Utils.h"

using namespace godzilla;

namespace {

struct TestStruct {
    int i;
    float f;
    String str;
};

} // namespace

namespace godzilla {

template <>
void
RestartFile::write<TestStruct>(const String & path, const String & name, const TestStruct & data)
{
    write(path + name, "integer", data.i);
    write(path + name, "float", data.f);
    write(path + name, "string", data.str);
}

template <>
void
RestartFile::read<TestStruct>(const String & path, const String & name, TestStruct & data) const
{
    read<int>(path + name, "integer", data.i);
    read<float>(path + name, "float", data.f);
    read<String>(path + name, "string", data.str);
}

} // namespace godzilla

TEST(RestartFileTest, read_write)
{
    TestApp app;

    {
        RestartFile f("restart.h5", FileAccess::CREATE);
        EXPECT_EQ(f.file_name(), "restart.h5");
        EXPECT_TRUE(utils::ends_with(f.file_path(), "restart.h5"));

        f.write<String>("/", "greeting", "hello");

        f.write<int>("/group1", "integer", 1);
        f.write<float>("/group1", "float", 1876.);

        TestStruct data = { -5, -3.56, "world" };
        f.write("/group2", "my_data", data);
    }

    {
        RestartFile f("restart.h5", FileAccess::READ);
        EXPECT_EQ(f.read<String>("/", "greeting"), "hello");

        EXPECT_EQ(f.read<int>("/group1", "integer"), 1);
        EXPECT_EQ(f.read<float>("/group1", "float"), 1876.);

        TestStruct data;
        f.read<TestStruct>("/group2", "my_data", data);
        EXPECT_EQ(data.i, -5);
        EXPECT_FLOAT_EQ(data.f, -3.56);
        EXPECT_EQ(data.str, "world");
    }
}
