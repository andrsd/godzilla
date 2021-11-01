#pragma once

#include "problems/GPetscLinearProblem.h"

/// Linear Poisson solver in 2D
///
/// Poisson equation in 2D:
///
///     div(grad p) = f,  0 < x,y < 1
///     with
///       forcing function f = -cos(m*pi*x)*cos(n*pi*y),
///       Neuman boundary conditions
///        dp/dx = 0 for x = 0, x = 1.
///        dp/dy = 0 for y = 0, y = 1.
///
/// Based on PETSc example 50[0]
///
/// References
/// - [0] https://petsc.org/release/src/ksp/ksp/tutorials/ex50.c.html
class PoissonLinearProblem : public GPetscLinearProblem
{
public:
    PoissonLinearProblem(const InputParameters & parameters);
    ~PoissonLinearProblem();

protected:
    virtual const DM & getDM() override;
    virtual void setupProblem() override;
    virtual PetscErrorCode computeRhsCallback(Vec b) override;
    virtual PetscErrorCode computeOperatorsCallback(Mat A, Mat B) override;

    /// Structured grid
    DM da;
    ///
    PetscReal uu;
    ///
    PetscReal tt;
    /// Number of grid points in x diretion
    PetscInt M;
    /// Number of grid points in y diretion
    PetscInt N;
    /// Number of DOFs per node
    PetscInt dofs;
    /// Stencil width
    PetscInt stencil_width;

public:
    static InputParameters validParams();
};
