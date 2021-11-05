#pragma once

#include "base/Factory.h"
#include "gmock/gmock.h"
#include "grids/G1DLineMesh.h"
#include "base/GodzillaApp_test.h"


using namespace godzilla;

class MockG1DLineMesh : public G1DLineMesh
{
public:
    MockG1DLineMesh(const InputParameters & params) : G1DLineMesh(params) {}
};


class G1DLineMeshTest : public GodzillaAppTest {
protected:
    MockG1DLineMesh *
    g1dLineMesh(PetscReal xmin, PetscReal xmax)
    {
        const std::string class_name = "MockG1DLineMesh";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscReal>("xmin") = xmin;
        params.set<PetscReal>("xmax") = xmax;
        params.set<PetscInt>("nx") = 10;
        return Factory::create<MockG1DLineMesh>(class_name, "obj", params);
    }
};
