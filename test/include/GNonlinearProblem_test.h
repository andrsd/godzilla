#pragma once

#include "GNonlinearProblem.h"
#include "GodzillaApp_test.h"


using namespace godzilla;

class GTestPetscNonlinearProblem : public GNonlinearProblem {
public:
    GTestPetscNonlinearProblem(const InputParameters & params);
    virtual ~GTestPetscNonlinearProblem();

protected:
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;
};
