#pragma once

#include "ExplicitFELinearProblem.h"

using namespace godzilla;

/// Solvng heat equation using an explicit scheme:
///
/// du/dt - \Delta u + f = 0
///
class HeatEquationExplicit : public ExplicitFELinearProblem {
public:
    HeatEquationExplicit(const Parameters & parameters);
    virtual ~HeatEquationExplicit();

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;

    /// Polynomial order of the FE space
    PetscInt order;

public:
    static Parameters parameters();

    static const PetscInt temp_id = 0;
    static const PetscInt ffn_aux_id = 0;
};
