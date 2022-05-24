#pragma once

#include "FENonlinearProblem.h"

using namespace godzilla;

/// Solves Poisson equation using finite element method
///
class PoissonEquation : public FENonlinearProblem {
public:
    PoissonEquation(const InputParameters & parameters);
    virtual ~PoissonEquation();

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;

    /// Polynomial order of the FE space
    PetscInt p_order;

    /// ID for the "u" field
    const PetscInt iu;

    /// ID for the forcing function field
    const PetscInt affn;

public:
    static InputParameters valid_params();
};
