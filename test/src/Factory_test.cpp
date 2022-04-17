#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "petsc.h"

using namespace godzilla;

TEST(FactoryTest, valid_params_unreg_obj)
{
    EXPECT_DEATH(InputParameters & params = Factory::getValidParams("ASDF"),
                 "Getting validParams for object 'ASDF' failed.  Object is not registred.");
}

TEST(FactoryTest, create_unreg_obj)
{
    InputParameters params = emptyInputParameters();
    EXPECT_DEATH(Factory::create<Object>("ASDF", "name", params),
                 "Trying to create object of unregistered type 'ASDF'.");
}

TEST(FactoryTest, create_reg_obj)
{
    App app("test", MPI_COMM_WORLD);
}

TEST(FactoryTest, create_wrong_type)
{
    App app("test", MPI_COMM_WORLD);
}
