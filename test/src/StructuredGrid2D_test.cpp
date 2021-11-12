#include "GodzillaApp_test.h"
#include "StructuredGrid2D_test.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

registerObject(MockStructuredGrid2D);

TEST_F(StructuredGrid2DTest, ctor)
{
    auto obj = gMesh(3, 4);
    EXPECT_EQ(obj->getNx(), 3);
    EXPECT_EQ(obj->getNy(), 4);
}

TEST_F(StructuredGrid2DTest, create)
{
    auto obj = gMesh(3, 4);
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 2);
}
