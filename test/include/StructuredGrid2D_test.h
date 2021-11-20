#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "StructuredGrid2D.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockStructuredGrid2D : public StructuredGrid2D {
public:
    MockStructuredGrid2D(const InputParameters & params) : StructuredGrid2D(params) {}
};

class StructuredGrid2DTest : public GodzillaAppTest {
protected:
    MockStructuredGrid2D *
    gMesh(PetscInt nx, PetscInt ny)
    {
        const std::string class_name = "MockStructuredGrid2D";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscInt>("nx") = nx;
        params.set<PetscInt>("ny") = ny;
        return Factory::create<MockStructuredGrid2D>(class_name, "obj", params);
    }
};
