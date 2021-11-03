#pragma once

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
    MOCK_METHOD(void, out, (), (override));
};


class GExecutionerTest : public GodzillaAppTest {
protected:
    std::shared_ptr<MockGProblem>
    gProblem()
    {
        const std::string class_name = "MockGProblem";
        InputParameters params = factory().getValidParams(class_name);
        return factory().create<MockGProblem>(class_name, "problem", params);
    }

    std::shared_ptr<MockGExecutioner>
    gExecutioner(std::shared_ptr<MockGProblem> problem)
    {
        const std::string class_name = "MockGExecutioner";
        InputParameters params = factory().getValidParams(class_name);
        params.set<GProblem *>("_gproblem") = problem.get();
        return factory().create<MockGExecutioner>(class_name, "obj", params);
    }
};
