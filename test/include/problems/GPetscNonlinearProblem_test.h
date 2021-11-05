#pragma once

#include "problems/GPetscNonlinearProblem.h"
#include "base/GodzillaApp_test.h"


using namespace godzilla;

class GTestPetscNonlinearProblem : public GPetscNonlinearProblem {
public:
    GTestPetscNonlinearProblem(const InputParameters & params);
    virtual ~GTestPetscNonlinearProblem();

protected:
    DM dm;

    virtual const DM &getDM() override;
    virtual void setupProblem() override;
    virtual void out() override;
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;
};
