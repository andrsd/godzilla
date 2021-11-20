#pragma once

#include "FENonlinearProblem.h"
#include "InitialCondition.h"
#include "GodzillaApp_test.h"

namespace godzilla {
class Grid;
class GTestFENonlinearProblem;

/// Test fixture
class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            InputParameters & params = Factory::getValidParams(class_name);
            params.set<const App *>("_app") = this->app;
            params.set<PetscInt>("nx") = 2;
            this->grid = Factory::create<Grid>(class_name, "grid", params);
        }
        {
            const std::string class_name = "GTestFENonlinearProblem";
            InputParameters & params = Factory::getValidParams(class_name);
            params.set<const App *>("_app") = this->app;
            params.set<Grid *>("_grid") = this->grid;
            this->prob = Factory::create<GTestFENonlinearProblem>(class_name, "prob", params);
        }
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    godzilla::Grid * grid;
    GTestFENonlinearProblem * prob;
};

/// Test problem for simple FE solver
class GTestFENonlinearProblem : public FENonlinearProblem {
public:
    GTestFENonlinearProblem(const InputParameters & params);
    virtual ~GTestFENonlinearProblem();

protected:
    virtual void onSetFields() override;
    virtual void onSetWeakForm() override;

    /// ID for the "u" field
    PetscInt u_id;
};

/// Test problem for simple FE solver with 2 fields
class GTest2FieldsFENonlinearProblem : public GTestFENonlinearProblem {
public:
    GTest2FieldsFENonlinearProblem(const InputParameters & params);

protected:
    virtual void onSetFields() override;

    /// ID for the "v" field
    PetscInt v_id;
};

/// Test IC with 2 components
class GTest2CompIC : public InitialCondition {
public:
    GTest2CompIC(const InputParameters & params) : InitialCondition(params) {}

    virtual PetscInt
    getNumComponents() const
    {
        return 2;
    }

    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[])
    {
        u[0] = 0.;
        u[1] = 10.;
    }
};

} // namespace godzilla
