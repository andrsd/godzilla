#pragma once

#include "problems/GPetscNonlinearProblem.h"
#include "petscsnes.h"

class GGrid;

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class GPetscFENonlinearProblem : public GPetscNonlinearProblem
{
public:
    GPetscFENonlinearProblem(const InputParameters & parameters);
    virtual ~GPetscFENonlinearProblem();

protected:
    virtual void setupProblem() override;
    virtual void setupCallbacks() override;
    virtual void setInitialGuess() override;
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;
    virtual void out() override;

typedef
    PetscErrorCode PetscFunc(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

public:
    static InputParameters validParams();
};
