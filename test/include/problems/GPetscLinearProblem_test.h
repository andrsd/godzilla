#pragma once

#include "problems/GPetscLinearProblem.h"
#include "base/GodzillaApp_test.h"


using namespace godzilla;

class GPetscLinearProblemTest : public GodzillaAppTest {
protected:
};


class GTestPetscLinearProblem : public GPetscLinearProblem {
public:
    GTestPetscLinearProblem(const InputParameters & params);
    virtual ~GTestPetscLinearProblem();

protected:
    DM dm;

    virtual const DM &getDM() override;
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;
};
