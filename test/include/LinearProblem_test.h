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
        Parameters * params = Factory::get_parameters(class_name);
        params->set<PetscInt>("nx") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    Mesh *
    gMesh2d()
    {
        const std::string class_name = "RectangleMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<PetscInt>("nx") = 1;
        params->set<PetscInt>("ny") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    Mesh *
    gMesh3d()
    {
        const std::string class_name = "BoxMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<PetscInt>("nx") = 1;
        params->set<PetscInt>("ny") = 1;
        params->set<PetscInt>("nz") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    Problem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "G1DTestLinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        return this->app->build_object<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem2d(Mesh * mesh)
    {
        const std::string class_name = "G2DTestLinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        return this->app->build_object<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem3d(Mesh * mesh)
    {
        const std::string class_name = "G3DTestLinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        return this->app->build_object<Problem>(class_name, "problem", params);
    }
};

//

class G1DTestLinearProblem : public LinearProblem {
public:
    G1DTestLinearProblem(const Parameters & params);
    virtual ~G1DTestLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode compute_rhs_callback(Vec b) override;
    virtual PetscErrorCode compute_operators_callback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G2DTestLinearProblem : public LinearProblem {
public:
    G2DTestLinearProblem(const Parameters & params);
    virtual ~G2DTestLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode compute_rhs_callback(Vec b) override;
    virtual PetscErrorCode compute_operators_callback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G3DTestLinearProblem : public LinearProblem {
public:
    G3DTestLinearProblem(const Parameters & params);
    virtual ~G3DTestLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode compute_rhs_callback(Vec b) override;
    virtual PetscErrorCode compute_operators_callback(Mat A, Mat B) override;

    PetscSection s;
};
