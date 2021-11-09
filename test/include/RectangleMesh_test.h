#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "RectangleMesh.h"
#include "GodzillaApp_test.h"


using namespace godzilla;

class MockRectangleMesh : public RectangleMesh
{
public:
    MockRectangleMesh(const InputParameters & params) : RectangleMesh(params) {}
};


class RectangleMeshTest : public GodzillaAppTest {
protected:
    MockRectangleMesh *
    g2dRectangleMesh(
            const std::vector<PetscReal> & xmin,
            const std::vector<PetscReal> & xmax)
    {
        const std::string class_name = "MockRectangleMesh";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscReal>("xmin") = xmin[0];
        params.set<PetscReal>("xmax") = xmax[0];
        params.set<PetscInt>("nx") = 9;
        params.set<PetscReal>("ymin") = xmin[1];
        params.set<PetscReal>("ymax") = xmax[1];
        params.set<PetscInt>("ny") = 8;
        return Factory::create<MockRectangleMesh>(class_name, "obj", params);
    }
};
