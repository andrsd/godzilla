#pragma once

#include "godzilla/FENonlinearProblem.h"

/// Solves Poisson equation using finite element method
///
class PoissonEquation : public godzilla::FENonlinearProblem {
public:
    PoissonEquation(const godzilla::Parameters & pars);

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// Polynomial order of the FE space
    godzilla::Order p_order;
    /// ID for the "u" field
    godzilla::FieldID iu;
    /// ID for the forcing function field
    godzilla::FieldID affn;

public:
    static godzilla::Parameters parameters();
};
