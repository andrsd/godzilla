#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "FunctionIC.h"

using namespace godzilla;

TEST(FunctionICTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = FunctionIC::valid_params();
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    auto obj = app.build_object<InitialCondition>("FunctionIC", "name", params);
    obj->create();

    EXPECT_EQ(obj->get_field_id(), 0);
    EXPECT_EQ(obj->get_num_components(), 1);

    PetscInt dim = 3;
    PetscReal time = 2.;
    PetscReal x[] = { 1, 2, 3 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    __initial_condition_function(dim, time, x, Nc, u, obj);

    EXPECT_EQ(u[0], 12);
}
