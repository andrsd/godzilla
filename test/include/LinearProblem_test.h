#pragma once

#include "LinearProblem.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

//

class G1DTestPetscLinearProblem : public LinearProblem {
public:
    G1DTestPetscLinearProblem(const InputParameters & params);
    virtual ~G1DTestPetscLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G2DTestPetscLinearProblem : public LinearProblem {
public:
    G2DTestPetscLinearProblem(const InputParameters & params);
    virtual ~G2DTestPetscLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};

//

class G3DTestPetscLinearProblem : public LinearProblem {
public:
    G3DTestPetscLinearProblem(const InputParameters & params);
    virtual ~G3DTestPetscLinearProblem();
    virtual void create() override;

protected:
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    PetscSection s;
};
