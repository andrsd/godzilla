#pragma once

#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"

class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            auto pars = LineMesh::parameters();
            // clang-format off
            pars.set<godzilla::App *>("_app", this->app)
                .set<Int>("nx", 2);
            // clang-format on
            this->mesh = MeshFactory::create<LineMesh>(pars);
        }
        {
            auto pars = GTestFENonlinearProblem::parameters();
            pars.set<godzilla::App *>("_app", this->app)
                .set<std::string>("_type", "GTestFENonlinearProblem")
                .set<Mesh *>("mesh", this->mesh.get());
            this->prob = this->app->build_object<GTestFENonlinearProblem>("prob", pars);
        }
        this->app->set_problem(this->prob);
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    Qtr<Mesh> mesh;
    GTestFENonlinearProblem * prob;
};

class FENonlinear2FieldProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            auto pars = LineMesh::parameters();
            // clang-format off
            pars.set<godzilla::App *>("_app", this->app)
                .set<Int>("nx", 2);
            // clang-format on
            this->mesh = MeshFactory::create<LineMesh>(pars);
        }
        {
            auto pars = GTest2FieldsFENonlinearProblem::parameters();
            pars.set<godzilla::App *>("_app", this->app)
                .set<std::string>("_type", "GTest2FieldsFENonlinearProblem")
                .set<Mesh *>("mesh", this->mesh.get());
            this->prob = this->app->build_object<GTest2FieldsFENonlinearProblem>("prob", pars);
        }
        this->app->set_problem(this->prob);
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    Qtr<Mesh> mesh;
    GTest2FieldsFENonlinearProblem * prob;
};
