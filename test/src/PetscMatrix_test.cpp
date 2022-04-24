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

    MatDestroy(&mat);
}
