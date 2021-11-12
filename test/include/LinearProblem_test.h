#pragma once

#include "LinearProblem.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class GTestPetscLinearProblem : public LinearProblem {
public:
    GTestPetscLinearProblem(const InputParameters & params);
    virtual ~GTestPetscLinearProblem();

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;
};
