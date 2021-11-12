#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "StructuredGrid1D.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockStructuredGrid1D : public StructuredGrid1D {
public:
    MockStructuredGrid1D(const InputParameters & params) : StructuredGrid1D(params) {}
};

class StructuredGrid1DTest : public GodzillaAppTest {
protected:
    MockStructuredGrid1D *
    gMesh(PetscInt nx)
    {
        const std::string class_name = "MockStructuredGrid1D";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscInt>("nx") = nx;
        return Factory::create<MockStructuredGrid1D>(class_name, "obj", params);
    }
};
