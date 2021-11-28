#pragma once

#include "ImplicitFENonlinearProblem.h"
#include "InitialCondition.h"
#include "GodzillaApp_test.h"

namespace godzilla {
class Grid;
class GTestImplicitFENonlinearProblem;

/// Test fixture
class ImplicitFENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            InputParameters & params = Factory::getValidParams(class_name);
            params.set<PetscInt>("nx") = 2;
            this->grid = this->app->buildObject<Grid>(class_name, "grid", params);
        }
        {
            const std::string class_name = "GTestImplicitFENonlinearProblem";
            InputParameters & params = Factory::getValidParams(class_name);
            params.set<Grid *>("_grid") = this->grid;
            params.set<PetscReal>("start_time") = 0.;
            params.set<PetscReal>("end_time") = 20;
            params.set<PetscReal>("dt") = 5;
            this->prob =
                this->app->buildObject<GTestImplicitFENonlinearProblem>(class_name, "prob", params);
        }
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    godzilla::Grid * grid;
    GTestImplicitFENonlinearProblem * prob;
};

/// Test problem for simple FE solver
class GTestImplicitFENonlinearProblem : public ImplicitFENonlinearProblem {
public:
    GTestImplicitFENonlinearProblem(const InputParameters & params);
    virtual ~GTestImplicitFENonlinearProblem();

protected:
    virtual void onSetFields() override;
    virtual void onSetWeakForm() override;

    /// ID for the "u" field
    const PetscInt iu;
};

} // namespace godzilla
