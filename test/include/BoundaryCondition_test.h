#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "BoundaryCondition.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockBoundaryCondition : public BoundaryCondition {
public:
    MockBoundaryCondition(const InputParameters & params) : BoundaryCondition(params) {}

    MOCK_METHOD(PetscInt, getFieldID, (), (const));
    MOCK_METHOD(PetscInt, getNumComponents, (), (const));
    MOCK_METHOD((DMBoundaryConditionType), getBcType, (), (const));
    MOCK_METHOD(void,
                evaluate,
                (PetscInt, PetscReal, const PetscReal x[], PetscInt Nc, PetscScalar u[]),
                ());
    MOCK_METHOD(std::vector<PetscInt>, getComponents, (), (const));
    MOCK_METHOD(void,
                setUpCallback,
                (PetscDS ds, DMLabel label, PetscInt n_ids, const PetscInt ids[]));
};

class BoundaryConditionTest : public GodzillaAppTest {
protected:
    MockBoundaryCondition *
    gBoundaryCondition()
    {
        const std::string class_name = "MockBoundaryCondition";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<std::string>>("boundary") = { "side1" };
        return Factory::create<MockBoundaryCondition>(class_name, "obj", params);
    }
};
