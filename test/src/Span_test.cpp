#include "gmock/gmock.h"
#include "godzilla/Span.h"

using namespace godzilla;

TEST(SpanTest, ctor_empty)
{
    Span<Int> sp;
    EXPECT_EQ(sp.size(), 0);
    EXPECT_TRUE(sp.empty());
}

TEST(SpanTest, ctor_vector)
{
    std::vector<Int> v = { 3, 5, 7, 9 };
    Span<Int> sp(v);
    ASSERT_EQ(sp.size(), 4);
    EXPECT_EQ(sp[0], 3);
    EXPECT_EQ(sp[1], 5);
    EXPECT_EQ(sp[2], 7);
    EXPECT_EQ(sp[3], 9);
    EXPECT_FALSE(sp.empty());
}

TEST(SpanTest, ctor_array)
{
    std::array<Int, 3> v = { 3, 5, 9 };
    Span<Int> sp(v);
    ASSERT_EQ(sp.size(), 3);
    EXPECT_EQ(sp[0], 3);
    EXPECT_EQ(sp[1], 5);
    EXPECT_EQ(sp[2], 9);
}

TEST(SpanTest, ctor_copy)
{
    std::array<Int, 3> v = { 3, 5, 9 };
    Span<Int> sp1(v);
    auto sp2 = sp1;
    ASSERT_EQ(sp2.size(), 3);
    EXPECT_EQ(sp2[0], 3);
    EXPECT_EQ(sp2[1], 5);
    EXPECT_EQ(sp2[2], 9);
}

TEST(SpanTest, ctor_iters)
{
    std::array<Int, 3> v = { 3, 5, 9 };
    Span<Int> sp(v);
    std::vector<Int> val;
    for (auto v : sp) {
        val.push_back(v);
    }
    EXPECT_THAT(val, testing::ElementsAre(3, 5, 9));
}

TEST(SpanTest, front_back)
{
    std::array<Int, 3> v = { 3, 5, 9 };
    Span<Int> sp(v);
    EXPECT_EQ(sp.front(), 3);
    EXPECT_EQ(sp.back(), 9);
}
