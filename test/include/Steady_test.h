#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "Steady.h"
#include "Problem.h"
#include "Output.h"
#include "GodzillaApp_test.h"

class MockSteady : public Steady {
public:
    MockSteady(const InputParameters & params) : Steady(params) {}

    MOCK_METHOD(void, execute, (), (override));
};

class MockProblem : public Problem {
public:
    MockProblem(const InputParameters & params) : Problem(params) {}

    MOCK_METHOD(void, create, (), (override));
    MOCK_METHOD(void, solve, (), (override));
    MOCK_METHOD(bool, converged, (), (override));
    MOCK_METHOD((const DM &), getDM, (), (const, override));
    MOCK_METHOD((const Vec &), getSolutionVector, (), (const, override));
};

class MockOutput : public Output {
public:
    MockOutput(const InputParameters & params) : Output(params) {}

    MOCK_METHOD(void, output, (), (const, override));
};

class SteadyTest : public GodzillaAppTest {
protected:
    MockProblem *
    gProblem()
    {
        const std::string class_name = "MockProblem";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        return Factory::create<MockProblem>(class_name, "problem", params);
    }

    Steady *
    gExecutioner(MockProblem * problem)
    {
        const std::string class_name = "Steady";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<Problem *>("_problem") = problem;
        return Factory::create<Steady>(class_name, "obj", params);
    }

    MockOutput *
    gOutput(MockProblem * problem, const std::string & name)
    {
        const std::string class_name = "MockOutput";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<Problem *>("_problem") = problem;
        return Factory::create<MockOutput>(class_name, name, params);
    }
};
