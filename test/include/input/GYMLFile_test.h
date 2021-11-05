#pragma once

#include "base/Factory.h"
#include "gmock/gmock.h"
#include "input/GYMLFile.h"
#include "problems/GProblem.h"
#include "base/GodzillaApp_test.h"


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
    GTestProblem(const InputParameters & params) : GProblem(params) {}

    void create() override {}
    void solve() override {}
    bool converged() override { return false; }
    void out() override {}

public:
    static InputParameters validParams();
};
