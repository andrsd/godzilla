#include "gtest/gtest.h"
#include "base/Factory.h"
#include "base/App.h"
#include "grids/G1DLineMesh.h"
#include "petsc.h"


using namespace godzilla;

TEST(Factory, valid_params_unreg_obj)
{
    EXPECT_DEATH(
        InputParameters params = Factory::getValidParams("ASDF"),
        "Getting validParams for object 'ASDF' failed.  Object is not registred."
    );
}

TEST(Factory, create_unreg_obj)
{
    InputParameters params = emptyInputParameters();
    EXPECT_DEATH(
        Factory::create<Object>("ASDF", "name", params),
        "Trying to create object of unregistered type 'ASDF'."
    );
}

TEST(Factory, create_reg_obj)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters params = Factory::getValidParams("G1DLineMesh");
    params.set<PetscInt>("nx") = 1;
    params.set<const App *>("_app") = &app;
    auto obj = Factory::create<G1DLineMesh>("G1DLineMesh", "name", params);
}
