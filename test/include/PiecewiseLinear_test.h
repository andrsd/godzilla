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
        params.set<std::vector<PetscReal>>("x") = x;
        params.set<std::vector<PetscReal>>("y") = y;
        return this->app->buildObject<PiecewiseLinear>(class_name, name, params);
    }
};
