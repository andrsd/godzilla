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
RestartFile::write<TestStruct>(String path, String name, const TestStruct & data)
{
    auto pth = fmt::format("{}/{}", path, name);
    write(pth, "integer", data.i);
    write(pth, "float", data.f);
    write(pth, "string", data.str);
}

template <>
void
RestartFile::read<TestStruct>(String path, String name, TestStruct & data) const
{
    auto pth = fmt::format("{}/{}", path, name);
    read<int>(pth, "integer", data.i);
    read<float>(pth, "float", data.f);
    read<String>(pth, "string", data.str);
}

} // namespace godzilla

TEST(RestartFileTest, read_write)
{
    TestApp app;

    {
        RestartFile f("restart.h5", FileAccess::CREATE);
        EXPECT_EQ(f.file_name(), "restart.h5");
        EXPECT_EQ(f.file_path().extension(), ".h5");

        f.write<String>("/", "greeting", "hello");

        f.write<int>("/group1", "integer", 1);
        f.write<float>("/group1", "float", 1876.);

        TestStruct data = { -5, -3.56, "world" };
        f.write("/group2", "my_data", data);
    }

    {
        RestartFile f("restart.h5", FileAccess::READ);
        String greeting;
        f.read<String>("/", "greeting", greeting);
        EXPECT_EQ(greeting, "hello");

        int i;
        f.read<int>("/group1", "integer", i);
        EXPECT_EQ(i, 1);

        float f32;
        f.read<float>("/group1", "float", f32);
        EXPECT_EQ(f32, 1876.);

        TestStruct data;
        f.read<TestStruct>("/group2", "my_data", data);
        EXPECT_EQ(data.i, -5);
        EXPECT_FLOAT_EQ(data.f, -3.56);
        EXPECT_EQ(data.str, "world");
    }
}
