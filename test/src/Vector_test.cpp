#include "gmock/gmock.h"
#include <petscvec.h>
#include "godzilla/IndexSet.h"
#include "godzilla/Vector.h"
#include "godzilla/PerfLog.h"
#include "ExceptionTestMacros.h"
#include "mpicpp-lite/mpicpp-lite.h"

using namespace godzilla;
namespace mpi = mpicpp_lite;

TEST(VectorTest, assembly)
{
    // TODO: this should really be tested with MPI (n_proc > 1)
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.assembly_begin();
    v.assembly_end();
    v.destroy();
}

TEST(VectorTest, assemble)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.assemble();
    v.destroy();
}

TEST(VectorTest, set_up)
{
    Vector v(MPI_COMM_WORLD);
    v.set_sizes(3);
    v.set_up();
    EXPECT_EQ(v.get_local_size(), 3);
}

TEST(VectorTest, set_sizes)
{
    Vector v(MPI_COMM_WORLD);
    v.set_type(VECSEQ);
    v.set_sizes(3);
    EXPECT_EQ(v.get_size(), 3);
    EXPECT_EQ(v.get_local_size(), 3);

    EXPECT_THROW_MSG(
        v.set_sizes(PETSC_DECIDE, PETSC_DECIDE),
        "Calling Vector::set_sizes with n = PETSC_DECIDE and N = PETSC_DECIDE is not allowed.");
    v.destroy();
}

TEST(VectorTest, get_type)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    Vector v = Vector::create_seq(comm, 10);
    if (comm.size() == 1)
        EXPECT_EQ(v.get_type(), "seq");
}

TEST(VectorTest, get_size)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 10);
    EXPECT_EQ(v.get_size(), 10);
    EXPECT_EQ(v.get_local_size(), 10);
    v.destroy();
}

TEST(VectorTest, get_values)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_values({ 0, 1, 2 }, { 3, 5, 9 });
    std::vector<Scalar> vals(2);
    v.get_values({ 0, 2 }, vals);
    EXPECT_DOUBLE_EQ(vals[0], 3.);
    EXPECT_DOUBLE_EQ(vals[1], 9.);
    v.destroy();
}

TEST(VectorTest, set)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 10);
    v.set(21.);
    for (Int i = 0; i < 10; ++i)
        EXPECT_DOUBLE_EQ(v(i), 21.);
    v.destroy();
}

TEST(VectorTest, set_value_local)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value_local(0, 3.);
    v.set_value_local(1, 5.);
    v.set_value_local(2, 9.);

    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values_c)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    Int ix[3] = { 0, 1, 2 };
    Real vals[3] = { 3, 5, 9 };
    v.set_values(3, ix, vals);
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_values({ 0, 1, 2 }, { 3, 5, 9 });
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values_dense)
{
    // TODO: this should be tested with MPI n_proc > 1
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    DenseVector<Int, 3> idx({ 0, 1, 2 });
    DenseVector<Scalar, 3> vals({ 3, 5, 9 });

    v.set_values(idx, vals);
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values_dyn_dense)
{
    // TODO: this should be tested with MPI n_proc > 1
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    DynDenseVector<Int> idx(3);
    idx.set_values({ 0, 1, 2 });
    DynDenseVector<Scalar> vals(3);
    vals.set_values({ 3, 5, 9 });

    v.set_values(idx, vals);
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values_local)
{
    // TODO: this should be tested with MPI n_proc > 1
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_values_local({ 0, 1, 2 }, { 3, 5, 9 });
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values_local_dense)
{
    // TODO: this should be tested with MPI n_proc > 1
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    DenseVector<Int, 3> idx({ 0, 1, 2 });
    DenseVector<Scalar, 3> vals({ 3, 5, 9 });

    v.set_values_local(idx, vals);
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, set_values_local_dyn_dense)
{
    // TODO: this should be tested with MPI n_proc > 1
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    DynDenseVector<Int> idx(3);
    idx.set_values({ 0, 1, 2 });
    DynDenseVector<Scalar> vals(3);
    vals.set_values({ 3, 5, 9 });

    v.set_values_local(idx, vals);
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 9.);
    v.destroy();
}

TEST(VectorTest, assign)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value(0, 1.);
    v.set_value(1, 3.);
    v.set_value(2, 7.);
    Vector dup = v.duplicate();
    dup.assign(v);
    EXPECT_DOUBLE_EQ(dup(0), 1.);
    EXPECT_DOUBLE_EQ(dup(1), 3.);
    EXPECT_DOUBLE_EQ(dup(2), 7.);
    dup.destroy();
    v.destroy();
}

TEST(VectorTest, duplicate_ret)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value(0, 1.);
    v.set_value(1, 3.);
    v.set_value(2, 7.);
    Vector dup = v.duplicate();
    copy(v, dup);
    EXPECT_DOUBLE_EQ(dup(0), 1.);
    EXPECT_DOUBLE_EQ(dup(1), 3.);
    EXPECT_DOUBLE_EQ(dup(2), 7.);
    v.destroy();
}

TEST(VectorTest, sum)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value(0, 1.);
    v.set_value(1, 3.);
    v.set_value(2, 7.);
    EXPECT_DOUBLE_EQ(v.sum(), 11.);
    v.destroy();
}

TEST(VectorTest, zero)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value(0, 1.);
    v.set_value(1, 3.);
    v.set_value(2, 7.);
    v.zero();
    EXPECT_DOUBLE_EQ(v(0), 0.);
    EXPECT_DOUBLE_EQ(v(1), 0.);
    EXPECT_DOUBLE_EQ(v(2), 0.);
    v.destroy();
}

TEST(VectorTest, dot)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value(0, 1.);
    v.set_value(1, 3.);
    v.set_value(2, 7.);

    Vector u = Vector::create_seq(MPI_COMM_WORLD, 3);
    u.set_value(0, 3.);
    u.set_value(1, 5.);
    u.set_value(2, 2.);

    auto dp = dot(v, u);
    EXPECT_DOUBLE_EQ(dp, 32.);

    v.destroy();
    u.destroy();
}

TEST(VectorTest, get_restore_array)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    Scalar * arr = v.get_array();
    arr[0] = 3.;
    arr[1] = 5.;
    arr[2] = 7.;
    v.restore_array(arr);

    EXPECT_EQ(v(0), 3.);
    EXPECT_EQ(v(1), 5.);
    EXPECT_EQ(v(2), 7.);

    v.destroy();
}

TEST(VectorTest, abs)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_value(0, -3.);
    v.set_value(1, 5.);
    v.set_value(2, -7.);
    v.abs();
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), 5.);
    EXPECT_DOUBLE_EQ(v(2), 7.);
    v.destroy();
}

TEST(VectorTest, scale)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, -3.);
    v.set_value(1, 5.);
    v.scale(-1.);
    EXPECT_DOUBLE_EQ(v(0), 3.);
    EXPECT_DOUBLE_EQ(v(1), -5.);
    v.destroy();
}

TEST(VectorTest, normalize)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, 3.);
    v.set_value(1, 4.);
    v.normalize();
    EXPECT_DOUBLE_EQ(v(0), 0.6);
    EXPECT_DOUBLE_EQ(v(1), 0.8);
    v.destroy();
}

TEST(VectorTest, shift)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, 3.);
    v.set_value(1, 4.);
    v.shift(1.);
    EXPECT_DOUBLE_EQ(v(0), 4.);
    EXPECT_DOUBLE_EQ(v(1), 5);
    v.destroy();
}

TEST(VectorTest, min)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, 3.);
    v.set_value(1, 4.);
    EXPECT_DOUBLE_EQ(v.min(), 3.);
    v.destroy();
}

TEST(VectorTest, max)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, 3.);
    v.set_value(1, 4.);
    EXPECT_DOUBLE_EQ(v.max(), 4.);
    v.destroy();
}

#if PETSC_VERSION_LT(3, 20, 0)
TEST(VectorTest, chop)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, 3.);
    v.set_value(1, 4.);
    v.chop(3.5);
    EXPECT_DOUBLE_EQ(v(0), 0.);
    EXPECT_DOUBLE_EQ(v(1), 4.);
    v.destroy();
}
#endif

#if PETSC_VERSION_GE(3, 20, 0)
TEST(VectorTest, filter)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value(0, 3.);
    v.set_value(1, 4.);
    v.filter(3.5);
    EXPECT_DOUBLE_EQ(v(0), 0.);
    EXPECT_DOUBLE_EQ(v(1), 4.);
    v.destroy();
}
#endif

TEST(VectorTest, axpy)
{
    Vector y = Vector::create_seq(MPI_COMM_WORLD, 2);
    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    x.set_value(0, 2.);
    x.set_value(1, 5.);

    axpy(y, 3., x);
    EXPECT_DOUBLE_EQ(y(0), 9.);
    EXPECT_DOUBLE_EQ(y(1), 19.);
    x.destroy();
    y.destroy();
}

TEST(VectorTest, axpby)
{
    Vector y = Vector::create_seq(MPI_COMM_WORLD, 2);
    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    x.set_value(0, 2.);
    x.set_value(1, 5.);

    axpby(y, 3., 4., x);
    EXPECT_DOUBLE_EQ(y(0), 18.);
    EXPECT_DOUBLE_EQ(y(1), 31.);
    x.destroy();
    y.destroy();
}

TEST(VectorTest, aypx)
{
    Vector y = Vector::create_seq(MPI_COMM_WORLD, 2);
    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    x.set_value(0, 2.);
    x.set_value(1, 5.);

    aypx(y, 3., x);
    EXPECT_DOUBLE_EQ(y(0), 11.);
    EXPECT_DOUBLE_EQ(y(1), 17.);

    x.destroy();
    y.destroy();
}

TEST(VectorTest, waxpy)
{
    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    x.set_value(0, 2.);
    x.set_value(1, 5.);

    Vector y = Vector::create_seq(MPI_COMM_WORLD, 2);
    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector w = Vector::create_seq(MPI_COMM_WORLD, 2);
    waxpy(w, 3., x, y);
    EXPECT_DOUBLE_EQ(w(0), 9.);
    EXPECT_DOUBLE_EQ(w(1), 19.);

    x.destroy();
    y.destroy();
    w.destroy();
}

TEST(VectorTest, maxpy)
{
    Vector v1 = Vector::create_seq(MPI_COMM_WORLD, 2);
    v1.set_value(0, 2.);
    v1.set_value(1, 5.);

    Vector v2 = Vector::create_seq(MPI_COMM_WORLD, 2);
    v2.set_value(0, 3.);
    v2.set_value(1, 4.);

    Vector w = Vector::create_seq(MPI_COMM_WORLD, 2);
    w.zero();
    std::vector<Scalar> alpha = { 2, -1 };
    std::vector<Vector> x = { v1, v2 };
    w.maxpy(alpha, x);

    EXPECT_DOUBLE_EQ(w(0), 1.);
    EXPECT_DOUBLE_EQ(w(1), 6.);

    v1.destroy();
    v2.destroy();
    w.destroy();
}

TEST(VectorTest, axpbypcz)
{
    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    x.set_values(std::vector<Int>({ 0, 1 }), { 2., 5. });

    Vector y = Vector::create_seq(MPI_COMM_WORLD, 2);
    y.set_values(std::vector<Int>({ 0, 1 }), { 3., 4. });

    Vector w = Vector::create_seq(MPI_COMM_WORLD, 2);
    w.set_values(std::vector<Int>({ 0, 1 }), { 1., 1. });
    Real alpha = 2.;
    Real beta = 3.;
    Real gamma = 4;
    w.axpbypcz(alpha, beta, gamma, x, y);

    EXPECT_DOUBLE_EQ(w(0), 17.);
    EXPECT_DOUBLE_EQ(w(1), 26.);

    x.destroy();
    y.destroy();
    w.destroy();
}

TEST(VectorTest, pointwise_min)
{
    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    Vector y = x.duplicate();
    Vector w = x.duplicate();

    x.set_value(0, 2.);
    x.set_value(1, 5.);

    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector::pointwise_min(w, x, y);
    EXPECT_DOUBLE_EQ(w(0), 2.);
    EXPECT_DOUBLE_EQ(w(1), 4.);

    x.destroy();
    y.destroy();
    w.destroy();
}

TEST(VectorTest, pointwise_max)
{
    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    Vector y = x.duplicate();
    Vector w = x.duplicate();

    x.set_value(0, 2.);
    x.set_value(1, 5.);

    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector::pointwise_max(w, x, y);
    EXPECT_DOUBLE_EQ(w(0), 3.);
    EXPECT_DOUBLE_EQ(w(1), 5.);

    x.destroy();
    y.destroy();
    w.destroy();
}

TEST(VectorTest, pointwise_mult)
{
    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    Vector y = x.duplicate();
    Vector w = x.duplicate();

    x.set_value(0, 2.);
    x.set_value(1, 5.);

    y.set_value(0, 3.);
    y.set_value(1, 4.);

    Vector::pointwise_mult(w, x, y);
    EXPECT_DOUBLE_EQ(w(0), 6.);
    EXPECT_DOUBLE_EQ(w(1), 20.);

    x.destroy();
    y.destroy();
    w.destroy();
}

TEST(VectorTest, pointwise_divide)
{
    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    Vector y = x.duplicate();
    Vector w = x.duplicate();

    x.set_value(0, 8.);
    x.set_value(1, 18.);

    y.set_value(0, 2.);
    y.set_value(1, 3.);

    Vector::pointwise_divide(w, x, y);
    EXPECT_DOUBLE_EQ(w(0), 4.);
    EXPECT_DOUBLE_EQ(w(1), 6.);

    x.destroy();
    y.destroy();
    w.destroy();
}

TEST(VectorTest, view)
{
    ::testing::internal::CaptureStdout();

    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_value_local(0, 1.);
    v.set_value_local(1, 4.);
    v.assemble();
    v.view();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("type: seq"));
    EXPECT_THAT(output, testing::HasSubstr("1."));
    EXPECT_THAT(output, testing::HasSubstr("4."));
}

TEST(VectorTest, reciprocal)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 3);
    v.set_values({ 0, 1, 2 }, { 2, 3, 4 });
    v.reciprocal();
    EXPECT_DOUBLE_EQ(v(0), 1. / 2.);
    EXPECT_DOUBLE_EQ(v(1), 1. / 3.);
    EXPECT_DOUBLE_EQ(v(2), 1. / 4.);
}

TEST(VectorTest, sub_vector)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 5);
    v.set_values({ 0, 1, 2, 3, 4 }, { 2, 4, 6, 8, 10 });
    auto is = IndexSet::create_general(MPI_COMM_WORLD, { 0, 1, 3 });
    auto y = v.get_sub_vector(is);
    EXPECT_DOUBLE_EQ(y(0), 2.);
    EXPECT_DOUBLE_EQ(y(1), 4.);
    EXPECT_DOUBLE_EQ(y(2), 8.);
    v.restore_sub_vector(is, y);
}

TEST(VectorTest, norm)
{
    Vector v = Vector::create_seq(MPI_COMM_WORLD, 2);
    v.set_values(std::vector<Int>({ 0, 1 }), { 3, 4 });
    auto norm = v.norm(NORM_2);
    EXPECT_DOUBLE_EQ(norm, 5.);
    v.destroy();
}

TEST(VectorTest, copy_is)
{
    auto full = Vector::create_seq(MPI_COMM_WORLD, 5);
    full.set_values({ 0, 1, 2, 3, 4 }, { 1, 2, 3, 4, 5 });

    auto reduced = Vector::create_seq(MPI_COMM_WORLD, 3);
    reduced.zero();
    auto is = IndexSet::create_general(MPI_COMM_WORLD, { 0, 2, 3 });
    full.copy(is, SCATTER_REVERSE, reduced);

    EXPECT_DOUBLE_EQ(reduced(0), 1.);
    EXPECT_DOUBLE_EQ(reduced(1), 3.);
    EXPECT_DOUBLE_EQ(reduced(2), 4.);

    reduced.set_values({ 0, 1, 2 }, { -1, -3, -4 });
    full.copy(is, SCATTER_FORWARD, reduced);

    EXPECT_DOUBLE_EQ(full(0), -1.);
    EXPECT_DOUBLE_EQ(full(1), 2.);
    EXPECT_DOUBLE_EQ(full(2), -3.);
    EXPECT_DOUBLE_EQ(full(3), -4.);
    EXPECT_DOUBLE_EQ(full(4), 5.);
}
