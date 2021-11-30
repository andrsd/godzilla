#pragma once

#include "Grid.h"
#include "LinearProblem.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class LinearProblemTest : public GodzillaAppTest {
protected:
    Grid *
    gGrid1d()
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Grid *
    gGrid2d()
    {
        const std::string class_name = "RectangleMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        params.set<PetscInt>("ny") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Grid *
    gGrid3d()
    {
        const std::string class_name = "BoxMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        params.set<PetscInt>("ny") = 1;
        params.set<PetscInt>("nz") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Problem *
    gProblem1d(Grid * grid)
    {
        const std::string class_name = "G1DTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem2d(Grid * grid)
    {
        const std::string class_name = "G2DTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem3d(Grid * grid)
    {
        const std::string class_name = "G3DTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }
};

//

class G1DTestPetscLinearProblem : public LinearProblem {
public:
    G1DTestPetscLinearProblem(const InputParameters & params);
    virtual ~G1DTestPetscLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G2DTestPetscLinearProblem : public LinearProblem {
public:
    G2DTestPetscLinearProblem(const InputParameters & params);
    virtual ~G2DTestPetscLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G3DTestPetscLinearProblem : public LinearProblem {
public:
    G3DTestPetscLinearProblem(const InputParameters & params);
    virtual ~G3DTestPetscLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};
