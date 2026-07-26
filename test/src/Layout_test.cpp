#include "gmock/gmock.h"
#include "godzilla/Layout.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

TEST(LayoutTest, ctor_empty)
{
    TestApp app;
    Layout layout;
    EXPECT_TRUE(layout.is_null());
}

TEST(LayoutTest, find_owner)
{
    TestApp app;
    auto comm = app.get_comm();
    auto layout = Layout::create_from_sizes(comm, 5, 5);
    auto owner = layout.find_owner(2);
    EXPECT_EQ(owner, 0);
}

TEST(LayoutTest, find_owner_index)
{
    TestApp app;
    auto comm = app.get_comm();
    auto layout = Layout::create_from_sizes(comm, 5, PETSC_DECIDE);
    auto [owner, index] = layout.find_owner_index(2);
    EXPECT_EQ(owner, 0);
    EXPECT_EQ(index, 2);
}

TEST(LayoutTest, getters_setters)
{
    TestApp app;
    auto comm = app.get_comm();

    Layout layout;
    layout.create(comm);
    layout.set_block_size(2);
    layout.set_local_size(6);
    layout.set_size(6);
    layout.set_up();

    EXPECT_EQ(layout.get_block_size(), 2);
    EXPECT_EQ(layout.get_local_size(), 6);
    EXPECT_EQ(layout.get_size(), 6);

    auto rng = layout.get_range();
    EXPECT_EQ(rng.first(), 0);
    EXPECT_EQ(rng.last(), 6);
}

TEST(LayoutTest, compare)
{
    TestApp app;

    auto comm = app.get_comm();
    auto l1 = Layout::create_from_sizes(comm, 5, 5);
    auto l2 = Layout::create_from_sizes(comm, 6, 6);
    auto l3 = Layout::create_from_sizes(comm, 5, 5);

    EXPECT_FALSE(l1.compare(l2));
    EXPECT_TRUE(l1.compare(l3));
}
