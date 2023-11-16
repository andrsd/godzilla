#pragma once

#include "godzilla/ExplicitFELinearProblem.h"

using namespace godzilla;

/// Solving heat equation using an explicit scheme:
///
/// du/dt - \Delta u + f = 0
///
class HeatEquationExplicit : public ExplicitFELinearProblem {
public:
    explicit HeatEquationExplicit(const Parameters & parameters);
    void create() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    PetscInt temp_id;
    PetscInt ffn_aux_id;
    /// Polynomial order of the FE space
    PetscInt order;

public:
    static Parameters parameters();
};
