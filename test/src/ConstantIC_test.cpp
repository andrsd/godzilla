#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "ConstantInitialCondition.h"

using namespace godzilla;

TEST(ConstantICTest, api)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "test");

    Parameters params = ConstantInitialCondition::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::vector<Real>>("value") = { 3, 4, 5 };
    ConstantInitialCondition obj(params);

    EXPECT_EQ(obj.get_field_id(), -1);
    EXPECT_EQ(obj.get_num_components(), 3);

    Int dim = 2;
    Real time = 0.;
    Real x[] = { 0 };
    Int Nc = 3;
    Scalar u[] = { 0, 0, 0 };
    obj.evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 3);
    EXPECT_EQ(u[1], 4);
    EXPECT_EQ(u[2], 5);
}
