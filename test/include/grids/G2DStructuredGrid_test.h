#pragma once

#include "gmock/gmock.h"
#include "grids/G2DStructuredGrid.h"
#include "base/GodzillaApp_test.h"


class MockG2DStructuredGrid : public G2DStructuredGrid
{
public:
    MockG2DStructuredGrid(const InputParameters & params) : G2DStructuredGrid(params) {}
};


class G2DStructuredGridTest : public GodzillaAppTest {
protected:
    std::shared_ptr<MockG2DStructuredGrid>
    gMesh(PetscInt nx, PetscInt ny)
    {
        const std::string class_name = "MockG2DStructuredGrid";
        InputParameters params = factory().getValidParams(class_name);
        params.set<PetscInt>("nx") = nx;
        params.set<PetscInt>("ny") = ny;
        return factory().create<MockG2DStructuredGrid>(class_name, "obj", params);
    }
};
