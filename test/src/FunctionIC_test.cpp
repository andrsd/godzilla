#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "FunctionIC.h"

using namespace godzilla;

TEST(FunctionICTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = FunctionIC::validParams();
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    auto obj = app.buildObject<InitialCondition>("FunctionIC", "name", params);
    obj->create();

    EXPECT_EQ(obj->getFieldId(), 0);
    EXPECT_EQ(obj->getNumComponents(), 1);

    PetscInt dim = 3;
    PetscReal time = 2.;
    PetscReal x[] = { 1, 2, 3 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    __initial_condition_function(dim, time, x, Nc, u, obj);

    EXPECT_EQ(u[0], 12);
}
