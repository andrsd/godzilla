#pragma once

#include "Mesh.h"
#include "LinearProblem.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class LinearProblemTest : public GodzillaAppTest {
protected:
    Mesh *
    gMesh1d()
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        return this->app->buildObject<Mesh>(class_name, "mesh", params);
    }

    Mesh *
    gMesh2d()
    {
        const std::string class_name = "RectangleMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        params.set<PetscInt>("ny") = 1;
        return this->app->buildObject<Mesh>(class_name, "mesh", params);
    }

    Mesh *
    gMesh3d()
    {
        const std::string class_name = "BoxMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        params.set<PetscInt>("ny") = 1;
        params.set<PetscInt>("nz") = 1;
        return this->app->buildObject<Mesh>(class_name, "mesh", params);
    }

    Problem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "G1DTestLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const Mesh *>("_mesh") = mesh;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem2d(Mesh * mesh)
    {
        const std::string class_name = "G2DTestLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const Mesh *>("_mesh") = mesh;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem3d(Mesh * mesh)
    {
        const std::string class_name = "G3DTestLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const Mesh *>("_mesh") = mesh;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }
};

//

class G1DTestLinearProblem : public LinearProblem {
public:
    G1DTestLinearProblem(const InputParameters & params);
    virtual ~G1DTestLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G2DTestLinearProblem : public LinearProblem {
public:
    G2DTestLinearProblem(const InputParameters & params);
    virtual ~G2DTestLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G3DTestLinearProblem : public LinearProblem {
public:
    G3DTestLinearProblem(const InputParameters & params);
    virtual ~G3DTestLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};
