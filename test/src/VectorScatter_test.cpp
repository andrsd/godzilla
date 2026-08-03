// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/VectorScatter.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Vector.h"

using namespace godzilla;
using namespace testing;

TEST(VectorScatterTest, create)
{
    TestApp app;

    auto x = Vector::create_seq(app.get_comm(), 3);
    x.set_values({ 0, 1, 2 }, { 10., 20., 30. });
    x.assemble();

    auto y = Vector::create_seq(app.get_comm(), 2);
    y.set(0.);
    y.assemble();

    auto ix = IndexSet::create_general(app.get_comm(), { 0, 2 });
    auto iy = IndexSet::create_general(app.get_comm(), { 0, 1 });

    auto scatter = VectorScatter::create(x, ix, y, iy);
    EXPECT_TRUE(scatter);
    EXPECT_FALSE(scatter.is_null());

    scatter.begin(x, y, INSERT_VALUES, SCATTER_FORWARD);
    scatter.end(x, y, INSERT_VALUES, SCATTER_FORWARD);

    EXPECT_DOUBLE_EQ(y(0), 10.);
    EXPECT_DOUBLE_EQ(y(1), 30.);
}

TEST(VectorScatterTest, copy)
{
    TestApp app;

    auto x = Vector::create_seq(app.get_comm(), 3);
    auto y = Vector::create_seq(app.get_comm(), 2);
    auto ix = IndexSet::create_general(app.get_comm(), { 0, 2 });
    auto iy = IndexSet::create_general(app.get_comm(), { 0, 1 });

    auto scatter = VectorScatter::create(x, ix, y, iy);
    auto raw_copy = scatter.copy();
    EXPECT_TRUE(raw_copy != nullptr);

    VectorScatter scatter_copy(raw_copy);
    EXPECT_TRUE(scatter_copy);
}

TEST(VectorScatterTest, properties)
{
    testing::internal::CaptureStdout();

    TestApp app;

    auto x = Vector::create_seq(app.get_comm(), 3);
    auto y = Vector::create_seq(app.get_comm(), 3);
    auto ix = IndexSet::create_general(app.get_comm(), { 0, 1, 2 });
    auto iy = IndexSet::create_general(app.get_comm(), { 0, 2, 1 });

    auto scatter = VectorScatter::create(x, ix, y, iy);
    EXPECT_FALSE(scatter.get_merged());

    auto type = scatter.get_type();
    EXPECT_TRUE(type.length() > 0);

    scatter.set_type(type.c_str());
    scatter.set_up();

    scatter.view();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("[0] 0 <- (0,0)"));
    EXPECT_THAT(out, testing::HasSubstr("[0] 1 <- (0,2)"));
    EXPECT_THAT(out, testing::HasSubstr("[0] 2 <- (0,1)"));
}

TEST(VectorScatterTest, create_to_all)
{
    TestApp app;

    auto x = Vector::create_seq(app.get_comm(), 3);
    x.set_values({ 0, 1, 2 }, { 100., 200., 300. });
    x.assemble();

    auto [scatter, y] = VectorScatter::create_to_all(x);
    EXPECT_TRUE(scatter);
    EXPECT_TRUE(y);
    EXPECT_EQ(y.get_size(), 3);

    scatter.begin(x, y, INSERT_VALUES, SCATTER_FORWARD);
    scatter.end(x, y, INSERT_VALUES, SCATTER_FORWARD);

    EXPECT_DOUBLE_EQ(y(0), 100.);
    EXPECT_DOUBLE_EQ(y(1), 200.);
    EXPECT_DOUBLE_EQ(y(2), 300.);
}

TEST(VectorScatterTest, create_to_zero)
{
    TestApp app;

    auto x = Vector::create_seq(app.get_comm(), 3);
    x.set_values({ 0, 1, 2 }, { 400., 500., 600. });
    x.assemble();

    auto [scatter, y] = VectorScatter::create_to_zero(x);
    EXPECT_TRUE(scatter);
    EXPECT_TRUE(y);

    scatter.begin(x, y, INSERT_VALUES, SCATTER_FORWARD);
    scatter.end(x, y, INSERT_VALUES, SCATTER_FORWARD);

    if (app.get_comm().rank() == 0) {
        EXPECT_EQ(y.get_size(), 3);
        EXPECT_DOUBLE_EQ(y(0), 400.);
        EXPECT_DOUBLE_EQ(y(1), 500.);
        EXPECT_DOUBLE_EQ(y(2), 600.);
    }
    else {
        EXPECT_EQ(y.get_size(), 0);
    }
}
