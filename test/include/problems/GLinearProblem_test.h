#pragma once

#include "problems/GLinearProblem.h"
#include "base/GodzillaApp_test.h"


using namespace godzilla;

class GLinearProblemTest : public GodzillaAppTest {
protected:
};


class GTestPetscLinearProblem : public GLinearProblem {
public:
    GTestPetscLinearProblem(const InputParameters & params);
    virtual ~GTestPetscLinearProblem();

protected:
    DM dm;

    virtual const DM & getDM() const override;
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;
};
