#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include "godzilla/Qtr.h"

using namespace godzilla;

namespace {

struct Data {
    int i;
    double d;

    Data(int j, double e) : i(j), d(e) {}
};

struct BigData : public Data {
    float f;

    BigData(float f, double d, int i) : Data(i, d), f(f) {}
};

Qtr<double>
alloc_double(double d)
{
    return Qtr<double>::alloc(d);
}

void
accept_qptr_double(Qtr<double> && a)
{
    Qtr<double> my = std::move(a);
}

void
accept_big_data(Qtr<Data> && a)
{
    Qtr<Data> storage = std::move(a);
}

} // namespace

TEST(QtrTest, alloc)
{
    auto q = Qtr<int>::alloc(1);
    EXPECT_FALSE(q.is_null());
    EXPECT_EQ(*q, 1);
}

TEST(QtrTest, alloc_by_function)
{
    auto q = alloc_double(122.);
    EXPECT_DOUBLE_EQ(*q, 122.);
}

TEST(QtrTest, move_ctor)
{
    auto q = alloc_double(122.);
    accept_qptr_double(std::move(q));
}

TEST(QtrTest, move_assign)
{
    auto q = alloc_double(122.);
    accept_qptr_double(std::move(q));
}

TEST(QtrTest, move_assign_cast)
{
    auto q = Qtr<BigData>::alloc(123., 42, 1);
    accept_big_data(std::move(q));
}

TEST(QtrTest, release)
{
    auto q = Qtr<int>::alloc(1);
    auto r = Qtr<int>::alloc(2);
    q = std::move(r);
    EXPECT_FALSE(q.is_null());
    EXPECT_TRUE(r.is_null());
}

TEST(QtrTest, reset)
{
    auto q = Qtr<double>::alloc(123.);
    q.reset();
    EXPECT_TRUE(q.is_null());
}

TEST(QtrTest, swap)
{
    auto q = alloc_double(123.);
    auto r = alloc_double(321.);
    q.swap(r);
    EXPECT_DOUBLE_EQ(*r, 123.);
    EXPECT_DOUBLE_EQ(*q, 321.);
}

TEST(QtrTest, alloc_data)
{
    auto q = Qtr<Data>::alloc(1, 23.4);
    EXPECT_EQ(q->i, 1);
    EXPECT_DOUBLE_EQ(q->d, 23.4);
}

TEST(QtrTest, alloc_with_cast)
{
    Qtr<Data> d = Qtr<BigData>::alloc(42., 123., 5);
    EXPECT_FALSE(d.is_null());
}

TEST(QtrTest, op_bool)
{
    auto q = Qtr<int>::alloc(123);
    if (q)
        SUCCEED();
    else
        FAIL();
}

TEST(QtrTest, cast)
{
    auto bd = Qtr<BigData>::alloc(12., 34., 1);
    auto d = std::move(bd);
    EXPECT_FALSE(d.is_null());
    EXPECT_TRUE(bd.is_null());
}
