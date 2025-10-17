#pragma once

#include "godzilla/FENonlinearProblem.h"

using namespace godzilla;

/// Solves Poisson equation using finite element method
///
class PoissonEquation : public FENonlinearProblem {
public:
    PoissonEquation(const Parameters & pars);

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// Polynomial order of the FE space
    Order p_order;
    /// ID for the "u" field
    FieldID iu;
    /// ID for the forcing function field
    FieldID affn;

public:
    static Parameters parameters();
};
