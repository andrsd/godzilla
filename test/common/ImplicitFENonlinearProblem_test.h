#pragma once

#include "GodzillaApp_test.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "godzilla/LineMesh.h"
#include "godzilla/MeshFactory.h"

class ImplicitFENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();
        this->app->get_registry().add<GTestImplicitFENonlinearProblem>(
            "GTestImplicitFENonlinearProblem");

        {
            auto pars = this->app->make_parameters<LineMesh>();
            pars.set<Int>("nx", 2);
            this->mesh = MeshFactory::create<LineMesh>(pars);
        }
        {
            auto pars = this->app->make_parameters<GTestImplicitFENonlinearProblem>();
            pars.set<Ref<Mesh>>("mesh", ref(*mesh))
                .set<Real>("start_time", 0.)
                .set<Real>("end_time", 20)
                .set<Real>("dt", 5);
            this->app->make_problem<GTestImplicitFENonlinearProblem>(pars);
        }
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    Qtr<Mesh> mesh;
};
