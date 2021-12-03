#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "BoundaryCondition.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockBoundaryCondition : public BoundaryCondition {
public:
    MockBoundaryCondition(const InputParameters & params) : BoundaryCondition(params) {}

    MOCK_METHOD(PetscInt, getFieldId, (), (const));
    MOCK_METHOD(PetscInt, getNumComponents, (), (const));
    MOCK_METHOD((DMBoundaryConditionType), getBcType, (), (const));
    MOCK_METHOD(void,
                evaluate,
                (PetscInt, PetscReal, const PetscReal x[], PetscInt Nc, PetscScalar u[]),
                ());
    MOCK_METHOD(std::vector<PetscInt>, getComponents, (), (const));
    MOCK_METHOD(void, setUpCallback, ());
};

class BoundaryConditionTest : public GodzillaAppTest {
protected:
    MockBoundaryCondition *
    gBoundaryCondition()
    {
        const std::string class_name = "MockBoundaryCondition";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<std::string>("boundary") = "side1";
        return this->app->buildObject<MockBoundaryCondition>(class_name, "obj", params);
    }
};
