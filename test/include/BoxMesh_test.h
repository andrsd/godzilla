#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "BoxMesh.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockBoxMesh : public BoxMesh {
public:
    MockBoxMesh(const InputParameters & params) : BoxMesh(params) {}
};

class BoxMeshTest : public GodzillaAppTest {
protected:
    MockBoxMesh *
    g3dBoxMesh(const std::vector<PetscReal> & xmin, const std::vector<PetscReal> & xmax)
    {
        const std::string class_name = "MockBoxMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscReal>("xmin") = xmin[0];
        params.set<PetscReal>("xmax") = xmax[0];
        params.set<PetscInt>("nx") = 9;
        params.set<PetscReal>("ymin") = xmin[1];
        params.set<PetscReal>("ymax") = xmax[1];
        params.set<PetscInt>("ny") = 8;
        params.set<PetscReal>("zmin") = xmin[2];
        params.set<PetscReal>("zmax") = xmax[2];
        params.set<PetscInt>("nz") = 7;
        return this->app->buildObject<MockBoxMesh>(class_name, "obj", params);
    }
};
