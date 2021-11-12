#include "GodzillaApp_test.h"
#include "BoundaryCondition_test.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

registerObject(MockBoundaryCondition);

TEST_F(BoundaryConditionTest, api)
{
    auto bc = gBoundaryCondition();

    EXPECT_EQ(bc->getFieldId(), 0);
    EXPECT_EQ(bc->getBoundary().size(), 1);
    EXPECT_EQ(bc->getBoundary()[0], "side1");
}

TEST_F(BoundaryConditionTest, evaluate)
{
    auto bc = gBoundaryCondition();

    PetscInt dim = 2;
    PetscReal time = 0.;
    PetscReal x[] = { 0 };
    PetscInt Nc = 3;
    PetscScalar u[] = { 0, 0, 0 };
    EXPECT_CALL(*bc, evaluate(dim, time, x, Nc, u)).Times(1);
    __boundary_condition_function(dim, time, x, Nc, u, bc);
}
