#pragma once

#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/MeshObject.h"
#include "godzilla/InitialCondition.h"
#include "GodzillaApp_test.h"
#include "GTestImplicitFENonlinearProblem.h"

class ImplicitFENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        this->app->get_registry().add<GTestImplicitFENonlinearProblem>(
            "GTestImplicitFENonlinearProblem");

        {
            Parameters * params = this->app->get_parameters("LineMesh");
            params->set<Int>("nx") = 2;
            this->mesh = this->app->build_object<MeshObject>("mesh", params);
        }
        {
            Parameters * params = this->app->get_parameters("GTestImplicitFENonlinearProblem");
            params->set<MeshObject *>("_mesh_obj") = mesh;
            params->set<Real>("start_time") = 0.;
            params->set<Real>("end_time") = 20;
            params->set<Real>("dt") = 5;
            this->prob = this->app->build_object<GTestImplicitFENonlinearProblem>("prob", params);
        }
        this->app->set_problem(this->prob);
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    MeshObject * mesh;
    GTestImplicitFENonlinearProblem * prob;
};
