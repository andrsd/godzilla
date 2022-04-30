#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "PetscMatrix.h"
#include "petsc.h"

using namespace godzilla;

TEST(PetscMatrixTest, test)
{
    TestApp app;

    Mat mat;
    MatCreateSeqAIJ(app.get_comm(), 3, 3, 1, nullptr, &mat);
    MatSetOption(mat, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);

    PetscMatrix m(mat);

    m.add(0, 0, 1.5);
    m.add(1, 1, 4.);
    m.add(1, 1, 2.);
    m.add(2, 2, 8.);
    m.finish();
    EXPECT_EQ(m.get(0, 0), 1.5);
    EXPECT_EQ(m.get(1, 1), 6.);
    EXPECT_EQ(m.get(2, 2), 8.);

    m.zero();
    EXPECT_EQ(m.get(0, 0), 0.);
    EXPECT_EQ(m.get(1, 1), 0.);
    EXPECT_EQ(m.get(2, 2), 0.);

    m.zero();
    DenseMatrix<Scalar> dm(2);
    dm[0][0] = 3;
    dm[0][1] = 1;
    dm[1][0] = 5;
    dm[1][1] = 7;
    PetscInt rows[] = { 0, 1 };
    PetscInt cols[] = { 0, 2 };
    m.add(dm, rows, cols);
    m.finish();
    EXPECT_EQ(m.get(0, 0), 3.);
    EXPECT_EQ(m.get(0, 1), 0.);
    EXPECT_EQ(m.get(0, 2), 1.);
    EXPECT_EQ(m.get(1, 0), 5.);
    EXPECT_EQ(m.get(1, 1), 0.);
    EXPECT_EQ(m.get(1, 2), 7.);
    EXPECT_EQ(m.get(2, 0), 0.);
    EXPECT_EQ(m.get(2, 1), 0.);
    EXPECT_EQ(m.get(2, 2), 0.);

    MatDestroy(&mat);
}
