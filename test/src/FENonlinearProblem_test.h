#pragma once

#include "gmock/gmock.h"
#include "godzilla/MeshObject.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/InitialCondition.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"

class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        this->app->get_registry().add<GTestFENonlinearProblem>("GTestFENonlinearProblem");

        {
            Parameters * params = this->app->get_parameters("LineMesh");
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<MeshObject>("mesh", params);
        }
        {
            Parameters * params = this->app->get_parameters("GTestFENonlinearProblem");
            params->set<MeshObject *>("_mesh_obj") = this->mesh;
            this->prob = this->app->build_object<GTestFENonlinearProblem>("prob", params);
        }
        this->app->set_problem(this->prob);
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    MeshObject * mesh;
    GTestFENonlinearProblem * prob;
};

class FENonlinear2FieldProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        this->app->get_registry().add<GTest2FieldsFENonlinearProblem>(
            "GTest2FieldsFENonlinearProblem");

        {
            Parameters * params = this->app->get_parameters("LineMesh");
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<MeshObject>("mesh", params);
        }
        {
            Parameters * params = this->app->get_parameters("GTest2FieldsFENonlinearProblem");
            params->set<MeshObject *>("_mesh_obj") = this->mesh;
            this->prob = this->app->build_object<GTest2FieldsFENonlinearProblem>("prob", params);
        }
        this->app->set_problem(this->prob);
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    MeshObject * mesh;
    GTest2FieldsFENonlinearProblem * prob;
};
