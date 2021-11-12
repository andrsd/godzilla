#include "GodzillaApp_test.h"
#include "StructuredGrid1D_test.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

registerObject(MockStructuredGrid1D);

TEST_F(StructuredGrid1DTest, ctor)
{
    auto obj = gMesh(3);
    EXPECT_EQ(obj->getNx(), 3);
}

TEST_F(StructuredGrid1DTest, create)
{
    auto obj = gMesh(3);
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 1);
}
