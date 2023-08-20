#pragma once

#include "FENonlinearProblem.h"

using namespace godzilla;

/// Solves Poisson equation using finite element method
///
class PoissonEquation : public FENonlinearProblem {
public:
    PoissonEquation(const Parameters & parameters);
    ~PoissonEquation() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// Polynomial order of the FE space
    PetscInt p_order;
    /// ID for the "u" field
    PetscInt iu;
    /// ID for the forcing function field
    PetscInt affn;

public:
    static Parameters parameters();
};
