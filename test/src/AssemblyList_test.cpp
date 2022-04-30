#include "gtest/gtest.h"
#include "AssemblyList.h"

using namespace godzilla;

TEST(AssemblyListTest, ctor)
{
    AssemblyList al;
    EXPECT_EQ(al.idx, nullptr);
    EXPECT_EQ(al.dof, nullptr);
    EXPECT_EQ(al.coef, nullptr);
    EXPECT_EQ(al.cnt, 0);
    EXPECT_EQ(al.cap, 0);
}

TEST(AssemblyListTest, add_empty)
{
    AssemblyList al;
    al.add(1, 0, 0.);
    EXPECT_EQ(al.cnt, 0);
    EXPECT_EQ(al.cap, 0);
}

TEST(AssemblyListTest, add)
{
    AssemblyList al;
    al.add(1, 0, 1.);
    EXPECT_EQ(al.cnt, 1);
    EXPECT_EQ(al.idx[0], 1);
    EXPECT_EQ(al.dof[0], 0);
    EXPECT_EQ(al.coef[0], 1.);

    al.clear();
    EXPECT_EQ(al.cnt, 0);
}
