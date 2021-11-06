#pragma once

#include "base/Factory.h"
#include "gmock/gmock.h"
#include "executioners/GExecutioner.h"
#include "problems/GProblem.h"
#include "base/GodzillaApp_test.h"

class MockGExecutioner : public GExecutioner
{
public:
    MockGExecutioner(const InputParameters & params) : GExecutioner(params) {}
};


class MockGProblem : public GProblem
{
public:
    MockGProblem(const InputParameters & params) : GProblem(params) {}

    MOCK_METHOD(void, create, (), (override));
    MOCK_METHOD(void, solve, (), (override));
    MOCK_METHOD(bool, converged, (), (override));
    MOCK_METHOD((const DM &), getDM, (), (override));
};


class GExecutionerTest : public GodzillaAppTest {
protected:
    MockGProblem *
    gProblem()
    {
        const std::string class_name = "MockGProblem";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        return Factory::create<MockGProblem>(class_name, "problem", params);
    }

    MockGExecutioner *
    gExecutioner(MockGProblem * problem)
    {
        const std::string class_name = "MockGExecutioner";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<GProblem *>("_gproblem") = problem;
        return Factory::create<MockGExecutioner>(class_name, "obj", params);
    }
};
