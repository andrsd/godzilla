#pragma once

#include "FENonlinearProblem.h"

using namespace godzilla;

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class PoissonFENonlinearProblem : public FENonlinearProblem {
public:
    PoissonFENonlinearProblem(const InputParameters & parameters);
    virtual ~PoissonFENonlinearProblem();

protected:
    virtual void onSetFields() override;
    virtual void onSetWeakForm() override;

    /// Polynomial order of the FE space
    PetscInt p_order;

    /// Constant forcing function
    PetscReal ffn;

    /// ID for the "u" field
    const PetscInt iu;

public:
    static InputParameters validParams();
};
