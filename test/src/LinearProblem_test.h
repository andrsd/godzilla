#pragma once

#include "godzilla/MeshObject.h"
#include "godzilla/LinearProblem.h"
#include "godzilla/Section.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class G1DTestLinearProblem : public LinearProblem {
public:
    G1DTestLinearProblem(const Parameters & params);
    virtual ~G1DTestLinearProblem();
    virtual void create() override;
    virtual void solve() override;
    virtual ErrorCode compute_rhs(Vector & b) override;
    virtual ErrorCode compute_operators(Matrix & A, Matrix & B) override;

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
    virtual ErrorCode compute_rhs(Vector & b) override;
    virtual ErrorCode compute_operators(Matrix & A, Matrix & B) override;

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
    virtual ErrorCode compute_rhs(Vector & b) override;
    virtual ErrorCode compute_operators(Matrix & A, Matrix & B) override;

protected:
    Section s;
};

//

class LinearProblemTest : public GodzillaAppTest {
protected:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        this->app->get_registry().add<G1DTestLinearProblem>("G1DTestLinearProblem");
        this->app->get_registry().add<G2DTestLinearProblem>("G2DTestLinearProblem");
        this->app->get_registry().add<G3DTestLinearProblem>("G3DTestLinearProblem");
    }

    MeshObject *
    gMesh1d()
    {
        const std::string class_name = "LineMesh";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Int>("nx") = 1;
        return this->app->build_object<MeshObject>(class_name, "mesh", params);
    }

    MeshObject *
    gMesh2d()
    {
        const std::string class_name = "RectangleMesh";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Int>("nx") = 1;
        params->set<Int>("ny") = 1;
        return this->app->build_object<MeshObject>(class_name, "mesh", params);
    }

    MeshObject *
    gMesh3d()
    {
        const std::string class_name = "BoxMesh";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<Int>("nx") = 1;
        params->set<Int>("ny") = 1;
        params->set<Int>("nz") = 1;
        return this->app->build_object<MeshObject>(class_name, "mesh", params);
    }

    G1DTestLinearProblem *
    gProblem1d(MeshObject * mesh)
    {
        const std::string class_name = "G1DTestLinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<MeshObject *>("_mesh_obj") = mesh;
        return this->app->build_object<G1DTestLinearProblem>(class_name, "problem", params);
    }

    G2DTestLinearProblem *
    gProblem2d(MeshObject * mesh)
    {
        const std::string class_name = "G2DTestLinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<MeshObject *>("_mesh_obj") = mesh;
        return this->app->build_object<G2DTestLinearProblem>(class_name, "problem", params);
    }

    G3DTestLinearProblem *
    gProblem3d(MeshObject * mesh)
    {
        const std::string class_name = "G3DTestLinearProblem";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<MeshObject *>("_mesh_obj") = mesh;
        return this->app->build_object<G3DTestLinearProblem>(class_name, "problem", params);
    }
};
