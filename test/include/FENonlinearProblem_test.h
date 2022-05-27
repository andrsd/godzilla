#pragma once

#include "gmock/gmock.h"
#include "Mesh.h"
#include "FENonlinearProblem.h"
#include "InitialCondition.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "petsc.h"


/// Test fixture
class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            const std::string class_name = "LineMesh";
            InputParameters * params = Factory::get_valid_params(class_name);
            params->set<PetscInt>("nx") = 2;
            this->mesh = this->app->build_object<Mesh>(class_name, "mesh", params);
        }
        {
            const std::string class_name = "GTestFENonlinearProblem";
            InputParameters * params = Factory::get_valid_params(class_name);
            params->set<const Mesh *>("_mesh") = this->mesh;
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

    Mesh * mesh;
    GTestFENonlinearProblem * prob;
};
