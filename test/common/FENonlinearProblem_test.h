#pragma once

#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/String.h"

class FENonlinearProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            auto pars = godzilla::LineMesh::parameters();
            // clang-format off
            pars.set<godzilla::App *>("app", this->app)
                .set<godzilla::Int>("nx", 2);
            // clang-format on
            this->mesh = godzilla::MeshFactory::create<godzilla::LineMesh>(pars);
        }
        {
            auto pars = GTestFENonlinearProblem::parameters();
            pars.set<godzilla::App *>("app", this->app)
                .set<godzilla::String>("_type", "GTestFENonlinearProblem")
                .set<godzilla::Mesh *>("mesh", this->mesh.get());
            this->app->make_problem<GTestFENonlinearProblem>(pars);
        }
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    godzilla::Qtr<godzilla::Mesh> mesh;
};

class FENonlinear2FieldProblemTest : public GodzillaAppTest {
public:
    void
    SetUp() override
    {
        GodzillaAppTest::SetUp();

        {
            auto pars = godzilla::LineMesh::parameters();
            // clang-format off
            pars.set<godzilla::App *>("app", this->app)
                .set<godzilla::Int>("nx", 2);
            // clang-format on
            this->mesh = godzilla::MeshFactory::create<godzilla::LineMesh>(pars);
        }
        {
            auto pars = GTest2FieldsFENonlinearProblem::parameters();
            pars.set<godzilla::App *>("app", this->app)
                .set<godzilla::String>("_type", "GTest2FieldsFENonlinearProblem")
                .set<godzilla::Mesh *>("mesh", this->mesh.get());
            this->app->make_problem<GTest2FieldsFENonlinearProblem>(pars);
        }
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    godzilla::Qtr<godzilla::Mesh> mesh;
};
