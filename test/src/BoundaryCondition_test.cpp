#include "GodzillaApp_test.h"
#include "BoundaryCondition_test.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

registerObject(MockBoundaryCondition);

TEST_F(BoundaryConditionTest, api)
{
    auto bc = gBoundaryCondition();

    EXPECT_EQ(bc->getBoundary(), "side1");
}
