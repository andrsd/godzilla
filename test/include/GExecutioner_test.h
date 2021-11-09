#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "GExecutioner.h"
#include "Problem.h"
#include "GOutput.h"
#include "GodzillaApp_test.h"

class MockGExecutioner : public GExecutioner
{
public:
    MockGExecutioner(const InputParameters & params) : GExecutioner(params) {}
};


class MockProblem : public Problem
{
public:
    MockProblem(const InputParameters & params) : Problem(params) {}

    MOCK_METHOD(void, create, (), (override));
    MOCK_METHOD(void, solve, (), (override));
    MOCK_METHOD(bool, converged, (), (override));
    MOCK_METHOD((const DM &), getDM, (), (const, override));
    MOCK_METHOD((const Vec &), getSolutionVector, (), (const, override));
};

class MockGOutput : public GOutput
{
public:
    MockGOutput(const InputParameters & params) : GOutput(params) {}

    MOCK_METHOD(void, output, (), (const, override));
};


class GExecutionerTest : public GodzillaAppTest {
protected:
    MockProblem *
    gProblem()
    {
        const std::string class_name = "MockProblem";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        return Factory::create<MockProblem>(class_name, "problem", params);
    }

    MockGExecutioner *
    gExecutioner(MockProblem * problem)
    {
        const std::string class_name = "MockGExecutioner";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<Problem *>("_Problem") = problem;
        return Factory::create<MockGExecutioner>(class_name, "obj", params);
    }

    MockGOutput *
    gOutput(MockProblem * problem, const std::string & name)
    {
        const std::string class_name = "MockGOutput";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<Problem *>("_Problem") = problem;
        return Factory::create<MockGOutput>(class_name, name, params);
    }
};
