#include "gmock/gmock.h"
#include "godzilla/Matrix.h"

using namespace godzilla;

TEST(MatrixTest, create)
{
    Matrix m;
    m.create(MPI_COMM_WORLD);
    m.destroy();
}

TEST(MatrixTest, set_sizes)
{
    Matrix m;
    m.create(MPI_COMM_WORLD);
    m.set_sizes(2, 3);
    m.set_up();
    EXPECT_EQ(m.get_n_rows(), 2);
    EXPECT_EQ(m.get_n_cols(), 3);
    m.destroy();
}

TEST(MatrixTest, assembly)
{
    // TODO: this should really be tested with MPI (n_proc > 1)
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.assemble();
    m.destroy();
}

TEST(MatrixTest, assemble)
{
    // TODO: this should really be tested with MPI (n_proc > 1)
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.assemble();
    m.destroy();
}

TEST(MatrixTest, get_size)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 3, 6, 1);
    EXPECT_EQ(m.get_n_rows(), 3);
    EXPECT_EQ(m.get_n_cols(), 6);
    Int rows, cols;
    m.get_size(rows, cols);
    EXPECT_EQ(rows, 3);
    EXPECT_EQ(cols, 6);
    m.destroy();
}

TEST(MatrixTest, set_value)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 2);
    m.set_value(0, 0, 1.);
    m.set_value(0, 1, 2.);
    m.set_value(1, 0, 3.);
    m.set_value(1, 1, 4.);
    m.assemble();

    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);

    m.destroy();
}

TEST(MatrixTest, set_value_local)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 2);
    m.set_value_local(0, 0, 1.);
    m.set_value_local(0, 1, 2.);
    m.set_value_local(1, 0, 3.);
    m.set_value_local(1, 1, 4.);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);

    m.destroy();
}

TEST(MatrixTest, set_values)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 2);
    std::vector<Int> rows = { 0, 1 };
    std::vector<Int> cols = { 0, 1 };
    std::vector<Scalar> vals = { 1, 2, 3, 4 };
    m.set_values(rows, cols, vals);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);
    m.destroy();
}

TEST(MatrixTest, set_values_dense)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 2);
    DenseVector<Int, 2> rows({ 0, 1 });
    DenseVector<Int, 2> cols({ 0, 1 });
    DenseMatrix<Scalar, 2, 2> vals;
    vals.set_row(0, { 1, 2 });
    vals.set_row(1, { 3, 4 });
    m.set_values(rows, cols, vals);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);
    m.destroy();
}

TEST(MatrixTest, set_values_dyn_dense)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 3, 2);
    DynDenseVector<Int> rows(2);
    rows.set_values({ 0, 1});
    DynDenseVector<Int> cols(2);
    cols.set_values({ 0, 2 });
    DynDenseMatrix<Scalar> vals(2, 2);
    vals.set_row(0, { 1, 2 });
    vals.set_row(1, { 3, 4 });
    m.set_values(rows, cols, vals);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 2), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 2), 4.);
    m.destroy();
}

TEST(MatrixTest, set_values_local)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 2);
    std::vector<Int> rows = { 0, 1 };
    std::vector<Int> cols = { 0, 1 };
    std::vector<Scalar> vals = { 1, 2, 3, 4 };
    m.set_values_local(rows, cols, vals);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);
    m.destroy();
}

TEST(MatrixTest, set_values_local_dense)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 2);
    DenseVector<Int, 2> rows({ 0, 1 });
    DenseVector<Int, 2> cols({ 0, 1 });
    DenseMatrix<Scalar, 2, 2> vals;
    vals.set_row(0, { 1, 2 });
    vals.set_row(1, { 3, 4 });
    m.set_values_local(rows, cols, vals);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);
    m.destroy();
}

TEST(MatrixTest, set_values_local_dyn_dense)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 3, 2);
    DynDenseVector<Int> rows(2);
    rows.set_values({ 0, 1 });
    DynDenseVector<Int> cols(2);
    cols.set_values({ 0, 2 });
    DynDenseMatrix<Scalar> vals(2, 2);
    vals.set_row(0, { 1, 2 });
    vals.set_row(1, { 3, 4 });
    m.set_values_local(rows, cols, vals);
    m.assemble();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 2), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 2), 4.);
    m.destroy();
}

TEST(MatrixTest, mult)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.set_value(0, 0, 1.);
    m.set_value(1, 1, 1.);
    m.assemble();

    Vector x = Vector::create_seq(MPI_COMM_WORLD, 2);
    x.set_values({ 0, 1 }, { 3, 5 });

    Vector y;
    x.duplicate(y);

    m.mult(x, y);

    EXPECT_DOUBLE_EQ(y(0), 3.);
    EXPECT_DOUBLE_EQ(y(1), 5.);

    m.destroy();
}

TEST(MatrixTest, zero)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.set_value_local(0, 0, 1.);
    m.set_value_local(1, 1, 4.);
    m.assemble();

    m.zero();
    EXPECT_DOUBLE_EQ(m(0, 0), 0.);
    EXPECT_DOUBLE_EQ(m(1, 1), 0.);
}

TEST(MatrixTest, create_seq_aij_2)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, { 1, 2 });
    EXPECT_EQ(m.get_n_cols(), 2);
    EXPECT_EQ(m.get_type(), MATSEQAIJ);
    m.destroy();
}

TEST(MatrixTest, view)
{
    ::testing::internal::CaptureStdout();

    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.set_value_local(0, 0, 1.);
    m.set_value_local(1, 1, 4.);
    m.assemble();
    m.view();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("type: seqaij"));
    EXPECT_THAT(output, testing::HasSubstr("row 0: (0, 1.)"));
    EXPECT_THAT(output, testing::HasSubstr("row 1: (1, 4.)"));
}

TEST(MatrixTest, scale)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.set_value_local(0, 0, 1.);
    m.set_value_local(1, 1, 4.);
    m.assemble();

    m.scale(4.);
    EXPECT_DOUBLE_EQ(m(0, 0), 4.);
    EXPECT_DOUBLE_EQ(m(1, 1), 16.);
}

TEST(MatrixTest, transpose)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 3, 3, { 3, 1, 2 });
    m.set_value_local(0, 0, 1.);
    m.set_value_local(0, 1, 2.);
    m.set_value_local(0, 2, 3.);
    m.set_value_local(1, 1, 4.);
    m.set_value_local(2, 0, 5.);
    m.set_value_local(2, 2, 6.);
    m.assemble();

    m.transpose();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(1, 0), 2.);
    EXPECT_DOUBLE_EQ(m(2, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);
    EXPECT_DOUBLE_EQ(m(0, 2), 5.);
    EXPECT_DOUBLE_EQ(m(2, 2), 6.);
}
