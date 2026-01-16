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
            this->prob = godzilla::Qtr<GTestFENonlinearProblem>(
                this->app->build_object<GTestFENonlinearProblem>(pars));
        }
        this->app->set_problem(this->prob.get());
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    godzilla::Qtr<godzilla::Mesh> mesh;
    godzilla::Qtr<GTestFENonlinearProblem> prob;
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
            this->prob = godzilla::Qtr<GTest2FieldsFENonlinearProblem>(
                this->app->build_object<GTest2FieldsFENonlinearProblem>(pars));
        }
        this->app->set_problem(this->prob.get());
    }

    void
    TearDown() override
    {
        GodzillaAppTest::TearDown();
    }

    godzilla::Qtr<godzilla::Mesh> mesh;
    godzilla::Qtr<GTest2FieldsFENonlinearProblem> prob;
};
