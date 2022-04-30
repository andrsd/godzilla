#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "DirichletBC.h"

using namespace godzilla;

TEST(DirichletBCTest, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters & params = Factory::get_valid_params("DirichletBC");
    params.set<std::vector<std::string>>("value") = { "t * x" };
    auto obj = app.build_object<DirichletBC>("DirichletBC", "name", params);
    obj->create();

    EXPECT_EQ(obj->get_bc_type(), BC_ESSENTIAL);
    EXPECT_EQ(obj->evaluate(2.5, 3, 5, 7), 7.5);
}
