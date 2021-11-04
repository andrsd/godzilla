#pragma once

#include "problems/GPetscNonlinearProblem.h"
#include "petscsnes.h"

using namespace godzilla;

/// Non-linear Poisson solver in 2D
///
/// Based on PETSc example 35[0]
///
/// References:
/// - [0] https://petsc.org/release/src/snes/tutorials/ex35.c.html
class PoissonNonlinearProblem : public GPetscNonlinearProblem
{
public:
    PoissonNonlinearProblem(const InputParameters & parameters);
    virtual ~PoissonNonlinearProblem();

protected:
    virtual void setupProblem() override;
    virtual void solve() override;
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;
    PetscErrorCode formMatrix(Mat jac);

    /// Jacobian matrix used to evaluate f = J * x
    Mat Jr;
    /// Constant part of the F(x) = b
    Vec b;

public:
    static InputParameters validParams();
};
