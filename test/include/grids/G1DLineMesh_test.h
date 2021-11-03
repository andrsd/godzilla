#pragma once

#include "gmock/gmock.h"
#include "grids/G1DLineMesh.h"
#include "base/GodzillaApp_test.h"


class MockG1DLineMesh : public G1DLineMesh
{
public:
    MockG1DLineMesh(const InputParameters & params) : G1DLineMesh(params) {}
};


class G1DLineMeshTest : public GodzillaAppTest {
protected:
    std::shared_ptr<MockG1DLineMesh>
    g1dLineMesh(PetscReal xmin, PetscReal xmax)
    {
        const std::string class_name = "MockG1DLineMesh";
        InputParameters params = factory().getValidParams(class_name);
        params.set<PetscReal>("xmin") = xmin;
        params.set<PetscReal>("xmax") = xmax;
        params.set<PetscInt>("nx") = 10;
        return factory().create<MockG1DLineMesh>(class_name, "obj", params);
    }
};
