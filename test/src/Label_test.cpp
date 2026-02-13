#include "gmock/gmock.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Label.h"
#include "TestApp.h"

using namespace testing;
using namespace godzilla;

TEST(Label, create_destroy)
{
    TestApp app;
    Label l;
    EXPECT_FALSE(l);
    EXPECT_EQ(l.is_null(), true);
    l.create(app.get_comm(), "name");
    EXPECT_EQ(l.is_null(), false);
    EXPECT_TRUE(l);
}

TEST(Label, default_value)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_default_value(-10);
    EXPECT_EQ(l.get_value(0), -10);
    EXPECT_EQ(l.get_default_value(), -10);
}

TEST(Label, set_value)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(1, 101);
    EXPECT_EQ(l.get_value(1), 101);
}

TEST(Label, reset)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(100, 1001);
    l.reset();
    EXPECT_EQ(l.get_value(100), -1);
    EXPECT_EQ(l.get_num_values(), 0);
}

TEST(Label, get_num_values)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(0, 1001);
    l.set_value(1, 1001);
    l.set_value(2, 1002);
    l.set_value(3, 1003);
    l.set_value(4, 1003);
    EXPECT_EQ(l.get_num_values(), 3);
}

TEST(Label, get_values)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(0, 1001);
    l.set_value(1, 1001);
    l.set_value(2, 1002);
    l.set_value(3, 1003);
    l.set_value(4, 1003);

    auto vals = l.get_values();
    EXPECT_THAT(vals, UnorderedElementsAre(1001, 1002, 1003));
}

TEST(Label, set_stratum)
{
    TestApp app;
    IndexSet is = IndexSet::create_general(app.get_comm(), { 1, 2, 3, 4, 5 });
    Label l;
    l.create(app.get_comm(), "name");
    l.set_stratum(101, is);
    EXPECT_EQ(l.get_value(1), 101);
    EXPECT_EQ(l.get_value(2), 101);
    EXPECT_EQ(l.get_value(3), 101);
    EXPECT_EQ(l.get_value(4), 101);
    EXPECT_EQ(l.get_value(5), 101);
    EXPECT_EQ(l.get_stratum_size(101), 5);
}

TEST(Label, get_stratum)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(1, 101);
    l.set_value(3, 101);
    l.set_value(4, 101);
    l.set_value(7, 101);
    EXPECT_EQ(l.get_stratum_size(101), 4);
    auto is = l.get_stratum(101);
    auto vals = is.borrow_indices();
    EXPECT_THAT(to_std_vector(vals), ::UnorderedElementsAre(1, 3, 4, 7));
}

TEST(Label, get_stratum_bounds)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(1, 101);
    l.set_value(3, 101);
    l.set_value(4, 101);
    l.set_value(7, 101);
    auto [first, last] = l.get_stratum_bounds(101);
    EXPECT_EQ(first, 1);
    EXPECT_EQ(last, 8);
}

TEST(Label, get_stratum_range)
{
    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(1, 101);
    l.set_value(3, 101);
    l.set_value(4, 101);
    l.set_value(7, 101);
    auto rng = l.get_stratum_range(101);
    EXPECT_EQ(rng.first(), 1);
    EXPECT_EQ(rng.last(), 8);
}

TEST(Label, view)
{
    testing::internal::CaptureStdout();

    TestApp app;
    Label l;
    l.create(app.get_comm(), "name");
    l.set_value(1, 101);
    l.set_value(3, 101);
    l.set_value(4, 102);
    l.set_value(7, 103);
    l.view();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("[0]: 1 (101)"));
    EXPECT_THAT(output, testing::HasSubstr("[0]: 3 (101)"));
    EXPECT_THAT(output, testing::HasSubstr("[0]: 4 (102)"));
    EXPECT_THAT(output, testing::HasSubstr("[0]: 7 (103)"));
}

TEST(Label, oper_bool)
{
    TestApp app;

    Label lbl;
    const auto & clbl = lbl;

    EXPECT_FALSE(lbl);
    EXPECT_FALSE(clbl);

    lbl.create(app.get_comm(), "name");
    EXPECT_TRUE(lbl);
    EXPECT_TRUE(clbl);
}
