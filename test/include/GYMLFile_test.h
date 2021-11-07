#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "GYMLFile.h"
#include "GProblem.h"
#include "GodzillaApp_test.h"
#include "petsc.h"


using namespace godzilla;

class MockGYMLFile : public GYMLFile
{
public:
    MockGYMLFile(const App & app) : GYMLFile(app) {}

    MOCK_METHOD(void, buildGrid, (), ());
    MOCK_METHOD(void, buildProblem, (), ());
    MOCK_METHOD(void, buildExecutioner, (), ());
};


class GYMLFileTest : public GodzillaAppTest {
protected:
    MockGYMLFile *
    gymlFile()
    {
        auto f = new MockGYMLFile(*this->app);
        return f;
    }
};


class GTestProblem : public GProblem
{
public:
    GTestProblem(const InputParameters & params) : GProblem(params)
    {
        DMPlexCreateBoxMesh(comm(), 1, PETSC_TRUE, NULL, NULL, NULL, NULL, PETSC_FALSE, &this->dm);
        DMSetUp(this->dm);
        DMCreateGlobalVector(this->dm, &this->x);
    }

    const DM & getDM() const override { return this->dm; }
    const Vec & getSolutionVector() const override { return this->x; }
    void create() override {}
    void solve() override {}
    bool converged() override { return false; }

protected:
    DM dm;
    Vec x;

public:
    static InputParameters validParams();
};
