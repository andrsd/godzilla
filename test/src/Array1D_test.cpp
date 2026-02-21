#include "gmock/gmock.h"
#include <petscvec.h>
#include "godzilla/Array1D.h"
#include "godzilla/DenseVector.h"
#include "godzilla/Range.h"

using namespace godzilla;
using namespace testing;

TEST(Array1DTest, create)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 10);
    EXPECT_EQ(arr.size(), 10);
}

TEST(Array1DTest, create_rng)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, godzilla::Range(5, 15));
    EXPECT_EQ(arr.size(), 10);
}

TEST(Array1DTest, const_op)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 10);
    for (Int i = 0; i < 10; ++i)
        arr[i] = i;

    const Array1D<Real> & ca = arr;
    for (Int i = 0; i < 10; ++i)
        EXPECT_EQ(ca[i], i);
}

TEST(Array1DTest, zero)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 10);
    for (Int i = 0; i < 10; ++i)
        arr[i] = i;
    arr.zero();
    for (Int i = 0; i < 10; ++i)
        EXPECT_EQ(arr[i], 0.);
}

TEST(Array1DTest, set_values)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 10);
    arr.set(1234);
    for (Int i = 0; i < 10; ++i)
        EXPECT_EQ(arr[i], 1234.);
}

TEST(Array1DTest, set_values_idxs)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 5);
    arr.zero();
    DenseVector<Int, 3> idxs({ 1, 4, 2 });
    DenseVector<Real, 3> vals({ 8, 7, 6 });
    set_values(arr, idxs, vals);
    EXPECT_EQ(arr[1], 8);
    EXPECT_EQ(arr[4], 7);
    EXPECT_EQ(arr[2], 6);
}

TEST(Array1DTest, get_data)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> y(comm, 5);
    assign(y, { 2., 3., 1., -2., 0. });
    Real * raw = y.get_data();
    EXPECT_EQ(raw[0], 2.);
    EXPECT_EQ(raw[1], 3.);
    EXPECT_EQ(raw[2], 1.);
    EXPECT_EQ(raw[3], -2.);
    EXPECT_EQ(raw[4], 0.);
}

TEST(Array1DTest, op_to_stream)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    testing::internal::CaptureStdout();

    Array1D<Real> x(comm, 5);
    assign(x, { 1., -1., 3., 0., 2. });
    std::cout << x;

    EXPECT_THAT(testing::internal::GetCapturedStdout(), testing::HasSubstr("(1, -1, 3, 0, 2)"));
}

TEST(Array1DTest, get_values)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 8);
    assign(x, { 2, 3, 1, -2, 0, 6, 10, 8 });

    DenseVector<Int, 3> idx({ 1, 6, 3 });
    auto vals = get_values(x, idx);

    EXPECT_EQ(vals(0), 3.);
    EXPECT_EQ(vals(1), 10.);
    EXPECT_EQ(vals(2), -2.);
}

TEST(Array1DTest, get_values_std_vec)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 8);
    assign(x, { 2, 3, 1, -2, 0, 6, 10, 8 });

    std::vector<Int> idx({ 1, 6, 3 });
    auto vals = get_values<Real, 3>(x, idx);

    EXPECT_EQ(vals(0), 3.);
    EXPECT_EQ(vals(1), 10.);
    EXPECT_EQ(vals(2), -2.);
}

TEST(Array1DTest, add)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 8);
    assign(x, { 2, 3, 1, -2, 0, 6, 10, 8 });

    DenseVector<Int, 3> idx({ 1, 6, 3 });
    DenseVector<Real, 3> dx({ -1, 2, 1 });
    add_values(x, idx, dx);

    EXPECT_EQ(x[0], 2.);
    EXPECT_EQ(x[1], 2.);
    EXPECT_EQ(x[2], 1.);
    EXPECT_EQ(x[3], -1.);
    EXPECT_EQ(x[4], 0.);
    EXPECT_EQ(x[5], 6.);
    EXPECT_EQ(x[6], 12.);
    EXPECT_EQ(x[7], 8.);
}

TEST(Array1DTest, range_ops)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 8);
    assign(arr, { 2, 3, 1, -2, 0, 6, 10, 8 });

    std::vector<Real> vals;
    for (auto & v : arr)
        vals.push_back(v);
    EXPECT_THAT(vals, ElementsAre(2, 3, 1, -2, 0, 6, 10, 8));
}

TEST(Array1DTest, assign_op)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> arr(comm, 8);
    arr.set(123.);
    auto copy = arr;

    for (auto i : copy)
        EXPECT_DOUBLE_EQ(i, 123.);
}

TEST(Array1DTest, norm)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> vals(comm, 5);
    assign(vals, { 1, -2, 3, -4, 5 });

    EXPECT_NEAR(norm(vals, NORM_1), 15, 1e-10);
    EXPECT_NEAR(norm(vals, NORM_2), 7.4161984871, 1e-10);
    EXPECT_NEAR(norm(vals, NORM_INFINITY), 5, 1e-10);

    EXPECT_THROW(norm(vals, NORM_1_AND_2), NotImplementedException);
    EXPECT_THROW(norm(vals, NORM_FROBENIUS), NotImplementedException);
}

TEST(Array1DTest, pointwise_min)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 5);
    assign(x, { 1, -2, 3, -4, 5 });
    Array1D<Real> y(comm, 5);
    assign(y, { 2, 1, -5, -4, 9 });

    Array1D<Real> w(comm, 5);
    pointwise_min(w, x, y);

    EXPECT_NEAR(w[0], 1., 1e-10);
    EXPECT_NEAR(w[1], -2., 1e-10);
    EXPECT_NEAR(w[2], -5., 1e-10);
    EXPECT_NEAR(w[3], -4., 1e-10);
    EXPECT_NEAR(w[4], 5., 1e-10);
}

TEST(Array1DTest, pointwise_max)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 5);
    assign(x, { 1, -2, 3, -4, 5 });
    Array1D<Real> y(comm, 5);
    assign(y, { 2, 1, -5, -4, 9 });

    Array1D<Real> w(comm, 5);
    pointwise_max(w, x, y);

    EXPECT_NEAR(w[0], 2., 1e-15);
    EXPECT_NEAR(w[1], 1., 1e-15);
    EXPECT_NEAR(w[2], 3., 1e-15);
    EXPECT_NEAR(w[3], -4., 1e-15);
    EXPECT_NEAR(w[4], 9., 1e-15);
}

TEST(Array1DTest, pointwise_mult)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 5);
    assign(x, { 1, -2, 3, -4, 5 });
    Array1D<Real> y(comm, 5);
    assign(y, { 2, 1, -5, -4, 9 });

    Array1D<Real> w(comm, 5);
    pointwise_mult(w, x, y);

    EXPECT_NEAR(w[0], 2., 1e-15);
    EXPECT_NEAR(w[1], -2., 1e-15);
    EXPECT_NEAR(w[2], -15., 1e-15);
    EXPECT_NEAR(w[3], 16., 1e-15);
    EXPECT_NEAR(w[4], 45., 1e-15);
}

TEST(Array1DTest, pointwise_divide)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 5);
    assign(x, { 1, -2, 6, -4, 5 });
    Array1D<Real> y(comm, 5);
    assign(y, { 2, 1, -2, -4, 20 });

    Array1D<Real> w(comm, 5);
    pointwise_divide(w, x, y);

    EXPECT_NEAR(w[0], 0.5, 1e-15);
    EXPECT_NEAR(w[1], -2., 1e-15);
    EXPECT_NEAR(w[2], -3., 1e-15);
    EXPECT_NEAR(w[3], 1., 1e-15);
    EXPECT_NEAR(w[4], 0.25, 1e-15);
}

TEST(Array1DTest, copy)
{
    mpi::Communicator comm(MPI_COMM_WORLD);

    Array1D<Real> x(comm, 5);
    assign(x, { 1., -2., 6., -4., 5. });
    Array1D<Real> y(comm, 5);
    copy(x, y);

    EXPECT_NEAR(y[0], 1., 1e-15);
    EXPECT_NEAR(y[1], -2., 1e-15);
    EXPECT_NEAR(y[2], 6., 1e-15);
    EXPECT_NEAR(y[3], -4., 1e-15);
    EXPECT_NEAR(y[4], 5., 1e-15);
}
