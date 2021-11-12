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

    /// ID for the "u" field
    PetscInt u_id;

public:
    static InputParameters validParams();
};
