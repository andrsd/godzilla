#include "GodzillaApp_test.h"
#include "G1DStructuredGrid_test.h"
#include "InputParameters.h"
#include "petsc.h"


using namespace godzilla;

registerObject(MockG1DStructuredGrid);

TEST_F(G1DStructuredGridTest, ctor)
{
    auto obj = gMesh(3);
    EXPECT_EQ(obj->getNx(), 3);
}

TEST_F(G1DStructuredGridTest, create)
{
    auto obj = gMesh(3);
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 1);
}
