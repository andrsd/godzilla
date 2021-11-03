#pragma once

#include "gmock/gmock.h"
#include "grids/G2DRectangleMesh.h"
#include "base/GodzillaApp_test.h"


class MockG2DRectangleMesh : public G2DRectangleMesh
{
public:
    MockG2DRectangleMesh(const InputParameters & params) : G2DRectangleMesh(params) {}
};


class G2DRectangleMeshTest : public GodzillaAppTest {
protected:
    std::shared_ptr<MockG2DRectangleMesh>
    g2dRectangleMesh(
            const std::vector<PetscReal> & xmin,
            const std::vector<PetscReal> & xmax)
    {
        const std::string class_name = "MockG2DRectangleMesh";
        InputParameters params = factory().getValidParams(class_name);
        params.set<PetscReal>("xmin") = xmin[0];
        params.set<PetscReal>("xmax") = xmax[0];
        params.set<PetscInt>("nx") = 9;
        params.set<PetscReal>("ymin") = xmin[1];
        params.set<PetscReal>("ymax") = xmax[1];
        params.set<PetscInt>("ny") = 8;
        return factory().create<MockG2DRectangleMesh>(class_name, "obj", params);
    }
};
