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
            auto pars = this->app->make_parameters<godzilla::LineMesh>();
            pars.set<godzilla::Int>("nx", 2);
            this->mesh = godzilla::MeshFactory::create<godzilla::LineMesh>(pars);
        }
        {
            auto pars = this->app->make_parameters<GTestFENonlinearProblem>();
            pars.set<godzilla::Ref<godzilla::Mesh>>("mesh", ref(*this->mesh));
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
            auto pars = this->app->make_parameters<godzilla::LineMesh>();
            pars.set<godzilla::Int>("nx", 2);
            this->mesh = godzilla::MeshFactory::create<godzilla::LineMesh>(pars);
        }
        {
            auto pars = this->app->make_parameters<GTest2FieldsFENonlinearProblem>();
            pars.set<godzilla::Ref<godzilla::Mesh>>("mesh", ref(*this->mesh));
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
