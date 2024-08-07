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

    Int temp_id;
    Int ffn_aux_id;
    /// Polynomial order of the FE space
    Int order;

public:
    static Parameters parameters();
};
