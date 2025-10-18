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
            auto pars = LineMesh::parameters();
            // clang-format off
            pars.set<godzilla::App *>("_app", this->app)
                .set<Int>("nx", 2);
            // clang-format on
            this->mesh = MeshFactory::create<LineMesh>(pars);
        }
        {
            auto pars = GTestImplicitFENonlinearProblem::parameters();
            pars.set<godzilla::App *>("_app", this->app)
                // FIXME: after app creates params and not pointers
                .set<std::string>("_type", "GTestImplicitFENonlinearProblem")
                .set<Mesh *>("mesh", mesh.get())
                .set<Real>("start_time", 0.)
                .set<Real>("end_time", 20)
                .set<Real>("dt", 5);
            this->prob = this->app->build_object<GTestImplicitFENonlinearProblem>("prob", pars);
        }
        this->app->set_problem(this->prob);
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    Qtr<Mesh> mesh;
    GTestImplicitFENonlinearProblem * prob;
};
