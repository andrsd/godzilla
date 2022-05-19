#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "ConstantIC.h"

using namespace godzilla;

TEST(ConstantICTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = ConstantIC::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::vector<PetscReal>>("value") = { 3, 4, 5 };
    ConstantIC obj(params);

    EXPECT_EQ(obj.get_field_id(), -1);
    EXPECT_EQ(obj.get_num_components(), 3);

    PetscInt dim = 2;
    PetscReal time = 0.;
    PetscReal x[] = { 0 };
    PetscInt Nc = 3;
    PetscScalar u[] = { 0, 0, 0 };
    __initial_condition_function(dim, time, x, Nc, u, &obj);

    EXPECT_EQ(u[0], 3);
    EXPECT_EQ(u[1], 4);
    EXPECT_EQ(u[2], 5);
}
