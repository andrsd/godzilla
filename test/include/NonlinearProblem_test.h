#pragma once

#include "NonlinearProblem.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class NonlinearProblemTest : public GodzillaAppTest {
protected:
    Grid *
    gGrid1d()
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Problem *
    gProblem1d(Grid * grid)
    {
        const std::string class_name = "G1DTestNonlinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }
};

//

class G1DTestNonlinearProblem : public NonlinearProblem {
public:
    G1DTestNonlinearProblem(const InputParameters & params);
    virtual ~G1DTestNonlinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;

    PetscSection s;
};
