#include "gtest/gtest.h"
#include "GodzillaApp_test.h"
#include "PetscVector.h"
#include "petsc.h"

using namespace godzilla;

TEST(PetscVectorTest, test)
{
    TestApp app;

    Vec vec;
    VecCreateSeq(app.get_comm(), 3, &vec);

    PetscVector v(vec);
    v.set(0, 8.);
    v.set(1, 7.);
    v.set(2, 6.);
    EXPECT_EQ(v.get(0), 8.);
    EXPECT_EQ(v.get(1), 7.);
    EXPECT_EQ(v.get(2), 6.);

    v.zero();
    EXPECT_EQ(v.get(0), 0.);
    EXPECT_EQ(v.get(1), 0.);
    EXPECT_EQ(v.get(2), 0.);

    v.add(1, 4.);
    v.add(1, 2.);
    EXPECT_EQ(v.get(1), 6.);

    v.set(1, 2.);
    EXPECT_EQ(v.get(1), 2.);

    const PetscInt ni[] = { 0, 2 };
    const PetscScalar y[] = { 3., 4. };
    v.add(2, ni, y);
    EXPECT_EQ(v.get(0), 3.);
    EXPECT_EQ(v.get(2), 4.);

    v.finish();

    VecDestroy(&vec);
}
