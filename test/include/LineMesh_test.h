#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "LineMesh.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockLineMesh : public LineMesh {
public:
    MockLineMesh(const InputParameters & params) : LineMesh(params) {}
};

class LineMeshTest : public GodzillaAppTest {
protected:
    MockLineMesh *
    g1dLineMesh(PetscReal xmin, PetscReal xmax)
    {
        const std::string class_name = "MockLineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscReal>("xmin") = xmin;
        params.set<PetscReal>("xmax") = xmax;
        params.set<PetscInt>("nx") = 10;
        return Factory::create<MockLineMesh>(class_name, "obj", params);
    }
};
