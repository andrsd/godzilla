#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "G1DStructuredGrid.h"
#include "GodzillaApp_test.h"


using namespace godzilla;

class MockG1DStructuredGrid : public G1DStructuredGrid
{
public:
    MockG1DStructuredGrid(const InputParameters & params) : G1DStructuredGrid(params) {}
};


class G1DStructuredGridTest : public GodzillaAppTest {
protected:
    MockG1DStructuredGrid *
    gMesh(PetscInt nx)
    {
        const std::string class_name = "MockG1DStructuredGrid";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscInt>("nx") = nx;
        return Factory::create<MockG1DStructuredGrid>(class_name, "obj", params);
    }
};
