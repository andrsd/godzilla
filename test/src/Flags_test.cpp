#include "gmock/gmock.h"
#include "godzilla/Flags.h"

namespace {

enum MyFlag : unsigned int { FLAG1 = 0x1, FLAG2 = 0x2, FLAG3 = 0x4 };
using MyFlags = godzilla::Flags<MyFlag>;

} // namespace

TEST(FlagsTest, no_flags)
{
    MyFlags flags;
    EXPECT_FALSE(flags.has_flags());
}

TEST(FlagsTest, ctor_flag)
{
    MyFlags flags(FLAG1);
    EXPECT_TRUE(flags & FLAG1);
    EXPECT_FALSE(flags & FLAG2);
    EXPECT_FALSE(flags & FLAG3);
}

TEST(FlagsTest, ctor_initializer_list)
{
    MyFlags flags({ FLAG1, FLAG3 });
    EXPECT_TRUE(flags & FLAG1);
    EXPECT_FALSE(flags & FLAG2);
    EXPECT_TRUE(flags & FLAG3);
}

TEST(FlagsTest, ctor_flag_or)
{
    MyFlags flags = FLAG1 | FLAG3;
    EXPECT_TRUE(flags & FLAG1);
    EXPECT_FALSE(flags & FLAG2);
    EXPECT_TRUE(flags & FLAG3);
}

TEST(FlagsTest, op_and)
{
    MyFlags flags(FLAG1);
    flags |= FLAG2;
    EXPECT_TRUE(flags & FLAG1);
    EXPECT_TRUE(flags & FLAG2);
    EXPECT_FALSE(flags & FLAG3);
}

TEST(FlagsTest, op_or)
{
    MyFlags flags(FLAG1);
    MyFlags flags2 = flags | FLAG3;
    EXPECT_TRUE(flags2 & FLAG1);
    EXPECT_FALSE(flags2 & FLAG2);
    EXPECT_TRUE(flags2 & FLAG3);
}
