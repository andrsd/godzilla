#pragma once

#include "gmock/gmock.h"
#include "FENonlinearProblem.h"
#include "InitialCondition.h"
#include "GodzillaApp_test.h"
#include "petsc.h"

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
            params.set<PetscInt>("nx") = 2;
            this->grid = this->app->buildObject<Grid>(class_name, "grid", params);
        }
        {
            const std::string class_name = "GTestFENonlinearProblem";
            InputParameters & params = Factory::getValidParams(class_name);
            params.set<Grid *>("_grid") = this->grid;
            this->prob =
                this->app->buildObject<GTestFENonlinearProblem>(class_name, "prob", params);
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

    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;
    const std::vector<PetscReal> &
    getConstants()
    {
        return this->consts;
    }

protected:
    virtual void onSetFields() override;
    virtual void onSetWeakForm() override;

    /// ID for the "u" field
    const PetscInt iu;
};

/// Test problem for simple FE solver with 2 fields
class GTest2FieldsFENonlinearProblem : public GTestFENonlinearProblem {
public:
    GTest2FieldsFENonlinearProblem(const InputParameters & params);

protected:
    virtual void onSetFields() override;

    /// ID for the "v" field
    const PetscInt iv;
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
