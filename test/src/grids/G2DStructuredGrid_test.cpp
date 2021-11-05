#include "base/GodzillaApp_test.h"
#include "grids/G2DStructuredGrid_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"


using namespace godzilla;

registerObject(MockG2DStructuredGrid);

TEST_F(G2DStructuredGridTest, ctor)
{
    auto obj = gMesh(3, 4);
    EXPECT_EQ(obj->getNx(), 3);
    EXPECT_EQ(obj->getNy(), 4);

    delete obj;
}

TEST_F(G2DStructuredGridTest, create)
{
    auto obj = gMesh(3, 4);
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 2);

    delete obj;
}
