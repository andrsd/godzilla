#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "G2DStructuredGrid.h"
#include "GodzillaApp_test.h"


using namespace godzilla;

class MockG2DStructuredGrid : public G2DStructuredGrid
{
public:
    MockG2DStructuredGrid(const InputParameters & params) : G2DStructuredGrid(params) {}
};


class G2DStructuredGridTest : public GodzillaAppTest {
protected:
    MockG2DStructuredGrid *
    gMesh(PetscInt nx, PetscInt ny)
    {
        const std::string class_name = "MockG2DStructuredGrid";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscInt>("nx") = nx;
        params.set<PetscInt>("ny") = ny;
        return Factory::create<MockG2DStructuredGrid>(class_name, "obj", params);
    }
};
