#pragma once

#include "GodzillaApp_test.h"
#include "PiecewiseLinear.h"

using namespace godzilla;

class PiecewiseLinearTest : public GodzillaAppTest {
protected:
    PiecewiseLinear *
    buildPiecewiseLinear(const std::string & name,
                         const std::vector<PetscReal> & x,
                         const std::vector<PetscReal> & y)
    {
        std::string class_name = "PiecewiseLinear";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::vector<PetscReal>>("x") = x;
        params.set<std::vector<PetscReal>>("y") = y;
        return Factory::create<PiecewiseLinear>(class_name, name, params);
    }
};
