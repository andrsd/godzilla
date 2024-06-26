#pragma once

#include "gmock/gmock.h"
#include "godzilla/MeshObject.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/InitialCondition.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "petsc.h"

class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        App::get_registry().add<GTestFENonlinearProblem>("GTestFENonlinearProblem");

        {
            const std::string class_name = "LineMesh";
            Parameters * params = this->app->get_parameters(class_name);
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<MeshObject>(class_name, "mesh", params);
        }
        {
            const std::string class_name = "GTestFENonlinearProblem";
            Parameters * params = this->app->get_parameters(class_name);
            params->set<MeshObject *>("_mesh_obj") = this->mesh;
            this->prob =
                this->app->build_object<GTestFENonlinearProblem>(class_name, "prob", params);
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
        App::get_registry().add<GTest2FieldsFENonlinearProblem>("GTest2FieldsFENonlinearProblem");

        {
            const std::string class_name = "LineMesh";
            Parameters * params = this->app->get_parameters(class_name);
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<MeshObject>(class_name, "mesh", params);
        }
        {
            const std::string class_name = "GTest2FieldsFENonlinearProblem";
            Parameters * params = this->app->get_parameters(class_name);
            params->set<MeshObject *>("_mesh_obj") = this->mesh;
            this->prob =
                this->app->build_object<GTest2FieldsFENonlinearProblem>(class_name, "prob", params);
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
