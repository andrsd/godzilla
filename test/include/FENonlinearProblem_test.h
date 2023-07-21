#pragma once

#include "gmock/gmock.h"
#include "UnstructuredMesh.h"
#include "FENonlinearProblem.h"
#include "InitialCondition.h"
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

        {
            const std::string class_name = "LineMesh";
            Parameters * params = Factory::get_parameters(class_name);
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<UnstructuredMesh>(class_name, "mesh", params);
        }
        {
            const std::string class_name = "GTestFENonlinearProblem";
            Parameters * params = Factory::get_parameters(class_name);
            params->set<Mesh *>("_mesh") = this->mesh;
            this->prob =
                this->app->build_object<GTestFENonlinearProblem>(class_name, "prob", params);
        }
        this->app->problem = this->prob;
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    UnstructuredMesh * mesh;
    GTestFENonlinearProblem * prob;
};

class FENonlinear2FieldProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            Parameters * params = Factory::get_parameters(class_name);
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<Mesh>(class_name, "mesh", params);
        }
        {
            const std::string class_name = "GTest2FieldsFENonlinearProblem";
            Parameters * params = Factory::get_parameters(class_name);
            params->set<Mesh *>("_mesh") = this->mesh;
            this->prob =
                this->app->build_object<GTest2FieldsFENonlinearProblem>(class_name, "prob", params);
        }
        this->app->problem = this->prob;
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    Mesh * mesh;
    GTest2FieldsFENonlinearProblem * prob;
};
