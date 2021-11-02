#pragma once

#include "problems/GPetscFENonlinearProblem.h"

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
    virtual void onSetFields() override;
    virtual void onSetWeakForm() override;
    virtual void onSetInitialConditions() override;
    virtual void out() override;

    virtual void setupBoundaryConditions() override;

    /// ID for the "u" field
    PetscInt u_id;

public:
    static InputParameters validParams();
};
