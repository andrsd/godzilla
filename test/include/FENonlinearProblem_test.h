#pragma once

#include "gmock/gmock.h"
#include "Mesh.h"
#include "FENonlinearProblem.h"
#include "InitialCondition.h"
#include "GodzillaApp_test.h"
#include "petsc.h"

using namespace godzilla;

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

    virtual void
    setUpConstants()
    {
        FENonlinearProblem::setUpConstants();
    }

    PetscDS
    getDS()
    {
        return this->ds;
    }

    void
    computePostprocessors() override
    {
        FENonlinearProblem::computePostprocessors();
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

/// Test fixture
class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            InputParameters * params = Factory::getValidParams(class_name);
            params->set<PetscInt>("nx") = 2;
            this->mesh = this->app->buildObject<Mesh>(class_name, "mesh", params);
        }
        {
            const std::string class_name = "GTestFENonlinearProblem";
            InputParameters * params = Factory::getValidParams(class_name);
            params->set<const Mesh *>("_mesh") = this->mesh;
            this->prob =
                this->app->buildObject<GTestFENonlinearProblem>(class_name, "prob", params);
        }
        this->app->problem = this->prob;
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    Mesh * mesh;
    GTestFENonlinearProblem * prob;
};
