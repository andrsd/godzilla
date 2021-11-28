#include "gtest/gtest.h"
#include "Factory.h"
#include "App.h"
#include "DirichletBC.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

TEST(DirichletBC, api)
{
    App app("test", MPI_COMM_WORLD);

    InputParameters & params = Factory::getValidParams("DirichletBC");
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    auto obj = app.buildObject<DirichletBC>("DirichletBC", "name", params);
    obj->create();

    EXPECT_EQ(obj->getFieldID(), 0);
    EXPECT_EQ(obj->getNumComponents(), 1);
    EXPECT_EQ(obj->getBcType(), DM_BC_ESSENTIAL);

    PetscInt dim = 3;
    PetscReal time = 2.5;
    PetscReal x[] = { 3, 5, 7 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    obj->evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 37.5);
}

TEST(DirichletBC, with_user_defined_fn)
{
    class TestApp : public App {
    public:
        TestApp() : App("test", MPI_COMM_WORLD) {}

        void
        addFunction(Function * fn)
        {
            this->functions.push_back(fn);
        }
    } app;

    Function * fn;
    DirichletBC * bc;
    {
        std::vector<PetscReal> x = { 0., 1. };
        std::vector<PetscReal> y = { 1., 2. };
        std::string class_name = "PiecewiseLinear";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<std::vector<PetscReal>>("x") = x;
        params.set<std::vector<PetscReal>>("y") = y;
        fn = app.buildObject<PiecewiseLinear>(class_name, "ipol", params);
        app.addFunction(fn);
    }

    {
        InputParameters & params = Factory::getValidParams("DirichletBC");
        params.set<std::vector<std::string>>("value") = { "ipol(x)" };
        bc = app.buildObject<DirichletBC>("DirichletBC", "name", params);
    }
    bc->create();

    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[] = { 0.5 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    bc->evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 1.5);
}
