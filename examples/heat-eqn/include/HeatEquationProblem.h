#pragma once

#include "ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Heat equation solver
///
class HeatEquationProblem : public ImplicitFENonlinearProblem {
public:
    explicit HeatEquationProblem(const Parameters & parameters);

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    PetscInt temp_id;
    PetscInt q_ppp_id;
    PetscInt htc_aux_id;
    PetscInt T_ambient_aux_id;
    /// Polynomial order of the FE space
    const PetscInt & p_order;

public:
    static Parameters parameters();
};
