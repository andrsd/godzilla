#pragma once

#include "problems/GPetscNonlinearProblem.h"
#include "petscsnes.h"

class GMesh;

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class GPetscFENonlinearProblem : public GPetscNonlinearProblem
{
public:
    GPetscFENonlinearProblem(const InputParameters & parameters);
    virtual ~GPetscFENonlinearProblem();

protected:
    virtual const DM & getDM() override;
    virtual void setupProblem() override;
    virtual void setupJacobian() override;
    virtual void setInitialGuess() override;
    virtual void out() override;
    /// Mesh
    GMesh & mesh;
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
