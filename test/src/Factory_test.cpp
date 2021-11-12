#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "LineMesh.h"
#include "petsc.h"

using namespace godzilla;

TEST(Factory, valid_params_unreg_obj)
{
    EXPECT_DEATH(InputParameters params = Factory::getValidParams("ASDF"),
                 "Getting validParams for object 'ASDF' failed.  Object is not registred.");
}

TEST(Factory, create_unreg_obj)
{
    InputParameters params = emptyInputParameters();
    EXPECT_DEATH(Factory::create<Object>("ASDF", "name", params),
                 "Trying to create object of unregistered type 'ASDF'.");
}

TEST(Factory, create_reg_obj)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = Factory::getValidParams("LineMesh");
    params.set<PetscInt>("nx") = 1;
    params.set<const App *>("_app") = &app;
    auto obj = Factory::create<LineMesh>("LineMesh", "name", params);
}
