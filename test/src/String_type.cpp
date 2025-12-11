#include "gmock/gmock.h"
#include "godzilla/String.h"

using namespace godzilla;

TEST(StringTest, to_lower)
{
    String str("asdf");
    EXPECT_EQ(str.to_lower(), "asdf");
}

TEST(StringTest, to_upper)
{
    String str("asdf");
    EXPECT_EQ(str.to_upper(), "ASDF");
}

TEST(StringTest, ends_with)
{
    String str("asdf");
    EXPECT_TRUE(str.ends_with("df"));
    EXPECT_FALSE(str.ends_with("long_string"));
    EXPECT_FALSE(str.ends_with("as"));
}

TEST(StringTest, starts_with)
{
    String str("asdf");
    EXPECT_TRUE(str.starts_with("as"));
    EXPECT_FALSE(str.starts_with("long_string"));
    EXPECT_FALSE(str.starts_with("df"));
}

TEST(StringTest, op_assign_str)
{
    String str;
    str = String("hello");
    EXPECT_EQ(str, "hello");
}

TEST(StringTest, op_assign_cchar)
{
    String str;
    str = "HELLO";
    EXPECT_EQ(str, "HELLO");
}
