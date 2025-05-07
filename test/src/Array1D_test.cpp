#include "gmock/gmock.h"
#include "godzilla/Array1D.h"
#include "godzilla/DenseVector.h"
#include "godzilla/Range.h"

using namespace godzilla;
using namespace testing;

TEST(Array1DTest, create)
{
    Array1D<Real> arr;
    arr.create(10);
    EXPECT_EQ(arr.get_size(), 10);
    arr.destroy();
}

TEST(Array1DTest, create_rng)
{
    Array1D<Real> arr;
    arr.create(godzilla::Range(5, 15));
    EXPECT_EQ(arr.get_size(), 10);
    arr.destroy();
}

TEST(Array1DTest, const_op)
{
    Array1D<Real> arr;
    arr.create(10);
    for (Int i = 0; i < 10; ++i)
        arr(i) = i;

    const Array1D<Real> & ca = arr;
    for (Int i = 0; i < 10; ++i)
        EXPECT_EQ(ca(i), i);

    arr.destroy();
}

TEST(Array1DTest, zero)
{
    Array1D<Real> arr(10);
    for (Int i = 0; i < 10; ++i)
        arr(i) = i;
    arr.zero();
    for (Int i = 0; i < 10; ++i)
        EXPECT_EQ(arr(i), 0.);
    arr.destroy();
}

TEST(Array1DTest, set_values)
{
    Array1D<Real> arr(10);
    arr.set(1234);
    for (Int i = 0; i < 10; ++i)
        EXPECT_EQ(arr(i), 1234.);
    arr.destroy();
}

TEST(Array1DTest, set_values_idxs)
{
    Array1D<Real> arr(5);
    arr.zero();
    DenseVector<Int, 3> idxs({ 1, 4, 2 });
    DenseVector<Real, 3> vals({ 8, 7, 6 });
    set_values(arr, idxs, vals);
    EXPECT_EQ(arr(1), 8);
    EXPECT_EQ(arr(4), 7);
    EXPECT_EQ(arr(2), 6);
    arr.destroy();
}

TEST(Array1DTest, get_data)
{
    Array1D<Real> y(5);
    y.set_values({ 2., 3., 1., -2., 0. });
    Real * raw = y.get_data();
    EXPECT_EQ(raw[0], 2.);
    EXPECT_EQ(raw[1], 3.);
    EXPECT_EQ(raw[2], 1.);
    EXPECT_EQ(raw[3], -2.);
    EXPECT_EQ(raw[4], 0.);
    y.destroy();
}

TEST(Array1DTest, op_to_stream)
{
    testing::internal::CaptureStdout();

    Array1D<Real> x(5);
    x.set_values({ 1., -1., 3., 0., 2. });
    std::cout << x;

    EXPECT_THAT(testing::internal::GetCapturedStdout(), testing::HasSubstr("(1, -1, 3, 0, 2)"));

    x.destroy();
}

TEST(Array1DTest, get_values)
{
    Array1D<Real> x(8);
    x.set_values({ 2, 3, 1, -2, 0, 6, 10, 8 });

    DenseVector<Int, 3> idx({ 1, 6, 3 });
    auto vals = get_values(x, idx);

    EXPECT_EQ(vals(0), 3.);
    EXPECT_EQ(vals(1), 10.);
    EXPECT_EQ(vals(2), -2.);

    x.destroy();
}

TEST(Array1DTest, get_values_std_vec)
{
    Array1D<Real> x(8);
    x.set_values({ 2, 3, 1, -2, 0, 6, 10, 8 });

    auto vals = get_values<Real, 3>(x, { 1, 6, 3 });

    EXPECT_EQ(vals(0), 3.);
    EXPECT_EQ(vals(1), 10.);
    EXPECT_EQ(vals(2), -2.);

    x.destroy();
}

TEST(Array1DTest, add)
{
    Array1D<Real> x(8);
    x.set_values({ 2, 3, 1, -2, 0, 6, 10, 8 });

    DenseVector<Int, 3> idx({ 1, 6, 3 });
    DenseVector<Real, 3> dx({ -1, 2, 1 });
    add_values(x, idx, dx);

    EXPECT_EQ(x(0), 2.);
    EXPECT_EQ(x(1), 2.);
    EXPECT_EQ(x(2), 1.);
    EXPECT_EQ(x(3), -1.);
    EXPECT_EQ(x(4), 0.);
    EXPECT_EQ(x(5), 6.);
    EXPECT_EQ(x(6), 12.);
    EXPECT_EQ(x(7), 8.);

    x.destroy();
}

TEST(Array1DTest, range_ops)
{
    Array1D<Real> arr(8);
    arr.set_values({ 2, 3, 1, -2, 0, 6, 10, 8 });

    std::vector<Real> vals;
    for (auto & v : arr)
        vals.push_back(v);
    EXPECT_THAT(vals, ElementsAre(2, 3, 1, -2, 0, 6, 10, 8));

    arr.destroy();
}
