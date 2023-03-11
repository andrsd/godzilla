#include "gmock/gmock.h"
#include "Matrix.h"

using namespace godzilla;

TEST(MatrixTest, create)
{
    Matrix m;
    m.create(MPI_COMM_WORLD);
    m.destroy();
}

TEST(MatrixTest, assembly)
{
    // TODO: this should really be tested with MPI (n_proc > 1)
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.assembly_begin();
    m.assembly_end();
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

    m.assembly_begin();
    m.assembly_end();

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

    m.assembly_begin();
    m.assembly_end();

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
    m.assembly_begin();
    m.assembly_end();
    EXPECT_DOUBLE_EQ(m(0, 0), 1.);
    EXPECT_DOUBLE_EQ(m(0, 1), 2.);
    EXPECT_DOUBLE_EQ(m(1, 0), 3.);
    EXPECT_DOUBLE_EQ(m(1, 1), 4.);
    m.destroy();
}

TEST(MatrixTest, mult)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.set_value(0, 0, 1.);
    m.set_value(1, 1, 1.);
    m.assembly_begin();
    m.assembly_end();

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
    m.assembly_begin();
    m.assembly_end();

    m.zero();
    EXPECT_DOUBLE_EQ(m(0, 0), 0.);
    EXPECT_DOUBLE_EQ(m(1, 1), 0.);
}

TEST(MatrixTest, create_seq_aij_2)
{
    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, { 1, 2 });
    EXPECT_EQ(m.get_n_cols(), 2);
    m.destroy();
}

TEST(MatrixTest, view)
{
    ::testing::internal::CaptureStdout();

    Matrix m = Matrix::create_seq_aij(MPI_COMM_WORLD, 2, 2, 1);
    m.set_value_local(0, 0, 1.);
    m.set_value_local(1, 1, 4.);
    m.assembly_begin();
    m.assembly_end();
    m.view();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("type: seqaij"));
    EXPECT_THAT(output, testing::HasSubstr("row 0: (0, 1.)"));
    EXPECT_THAT(output, testing::HasSubstr("row 1: (1, 4.)"));
}
