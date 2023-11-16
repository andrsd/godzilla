#pragma once

#include "Mesh.h"
#include "LinearProblem.h"
#include "Section.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class G1DTestLinearProblem : public LinearProblem {
public:
    G1DTestLinearProblem(const Parameters & params);
    virtual ~G1DTestLinearProblem();
    virtual void create() override;
    virtual void solve() override;
    virtual PetscErrorCode compute_rhs(Vector & b) override;
    virtual PetscErrorCode compute_operators(Matrix & A, Matrix & B) override;

protected:
    Section s;
};

//

class G2DTestLinearProblem : public LinearProblem {
public:
    G2DTestLinearProblem(const Parameters & params);
    virtual ~G2DTestLinearProblem();
    virtual void create() override;
    virtual void solve() override;
    virtual PetscErrorCode compute_rhs(Vector & b) override;
    virtual PetscErrorCode compute_operators(Matrix & A, Matrix & B) override;

protected:
    Section s;
};

//

class G3DTestLinearProblem : public LinearProblem {
public:
    G3DTestLinearProblem(const Parameters & params);
    virtual ~G3DTestLinearProblem();
    virtual void create() override;
    virtual void solve() override;
    virtual PetscErrorCode compute_rhs(Vector & b) override;
    virtual PetscErrorCode compute_operators(Matrix & A, Matrix & B) override;

protected:
    Section s;
};

//

class LinearProblemTest : public GodzillaAppTest {
protected:
    Mesh *
    gMesh1d()
    {
        const std::string class_name = "LineMesh";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Int>("nx") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    Mesh *
    gMesh2d()
    {
        const std::string class_name = "RectangleMesh";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Int>("nx") = 1;
        params->set<Int>("ny") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    Mesh *
    gMesh3d()
    {
        const std::string class_name = "BoxMesh";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Int>("nx") = 1;
        params->set<Int>("ny") = 1;
        params->set<Int>("nz") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    G1DTestLinearProblem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "G1DTestLinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        return this->app->build_object<G1DTestLinearProblem>(class_name, "problem", params);
    }

    G2DTestLinearProblem *
    gProblem2d(Mesh * mesh)
    {
        const std::string class_name = "G2DTestLinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        return this->app->build_object<G2DTestLinearProblem>(class_name, "problem", params);
    }

    G3DTestLinearProblem *
    gProblem3d(Mesh * mesh)
    {
        const std::string class_name = "G3DTestLinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        return this->app->build_object<G3DTestLinearProblem>(class_name, "problem", params);
    }
};
