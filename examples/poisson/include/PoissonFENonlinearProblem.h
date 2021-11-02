#pragma once

#include "problems/GPetscFENonlinearProblem.h"
#include "petscsnes.h"

class GGrid;

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class PoissonFENonlinearProblem : public GPetscFENonlinearProblem
{
public:
    PoissonFENonlinearProblem(const InputParameters & parameters);
    virtual ~PoissonFENonlinearProblem();

protected:
    virtual void setupProblem() override;
    virtual void setInitialGuess() override;
    virtual void out() override;

    /// Spatial dimension of the discrete problem
    PetscInt dim;
    /// FE object
    PetscFE fe;
    /// Object that manages a discrete system
    PetscDS ds;

typedef
    PetscErrorCode PetscFunc(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

    PetscFunc *initial_guess[1];

public:
    static InputParameters validParams();
};
