#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "LineMesh.h"
#include "RectangleMesh.h"
#include "petsc.h"

using namespace godzilla;

TEST(FactoryTest, valid_params_unreg_obj)
{
    EXPECT_DEATH(Parameters * params = Factory::get_parameters("ASDF"),
                 "Getting valid_params for object 'ASDF' failed.  Object is not registered.");
}

TEST(FactoryTest, create_unreg_obj)
{
    Parameters params;
    EXPECT_DEATH(Factory::create<Object>("ASDF", "name", params),
                 "Trying to create object of unregistered type 'ASDF'.");
}

TEST(FactoryTest, create_reg_obj)
{
    App app("test", MPI_COMM_WORLD);

    Parameters * params = Factory::get_parameters("LineMesh");
    params->set<Int>("nx") = 1;
    app.build_object<LineMesh>("LineMesh", "name", params);
}

TEST(FactoryTest, create_wrong_type)
{
    App app("test", MPI_COMM_WORLD);

    Parameters * params = Factory::get_parameters("LineMesh");
    params->set<Int>("nx") = 1;
    EXPECT_DEATH(app.build_object<RectangleMesh>("LineMesh", "name", params),
                 "\\[ERROR\\] Instantiation of object 'name:\\[LineMesh\\]' failed\\.");
}
